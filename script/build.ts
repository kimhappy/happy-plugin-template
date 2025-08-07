import { existsSync          } from 'fs'
import { copyFile, rm, mkdir } from 'fs/promises'
import { join                } from 'path'
import   yargs                 from 'yargs'
import { hideBin             } from 'yargs/helpers'
import   tailwind              from 'bun-plugin-tailwind'

const DSP_CHOICES    = ['Debug', 'Release'] as const
const VIEW_CHOICES   = ['hot'  , 'build'  ] as const
const SERVER_CHOICES = ['live' , 'bundle' ] as const

type Dsp    = typeof DSP_CHOICES   [ number ]
type View   = typeof VIEW_CHOICES  [ number ]
type Server = typeof SERVER_CHOICES[ number ]

const dspBuildDir = (
  dsp   : Dsp,
  server: Server
) => join('build', `${ dsp }-${ server }`)

const viewServePort = Bun.env.HAPPY_PORT ?? 3000
const viewSrcDir    = join('view' )
const viewAssetDir  = join('asset')
const viewBuildDir  = join('build', 'view')

const step = async (
  command: string,
  args   : string[] = []
) => {
  const proc = Bun.spawnSync(
    [command, ...args],
    { stdio: ['ignore', 'inherit', 'inherit'] })

  if (proc.signalCode)
    throw new Error(`Command failed with signal ${ proc.signalCode }: ${ command } ${ args.join(' ') }`)

  if (!proc.success)
    throw new Error(`Command failed with exit code ${ proc.exitCode }: ${ command } ${ args.join(' ') }`)
}

const dspConfigStep = async (
  dsp   : Dsp,
  server: Server
) => {
  const bd = dspBuildDir (dsp, server)

  if (existsSync(bd))
    return

  try {
    await mkdir(bd, { recursive: true })
    await step('cmake', [
      '-S', '.'                    ,
      '-B', bd                     ,
      '-G', 'Ninja'                ,
      `-DCMAKE_BUILD_TYPE=${ dsp }`,
      ...(server === 'bundle' ? [`-DVIEW_BUILD_DIR=${ viewBuildDir }`] : [])
    ])
    await dspClangdStep(dsp, server)
  }
  catch (error) {
    await rm(bd, { recursive: true, force: true })
    throw error
  }
}

const dspBuildStep = async (
  dsp   : Dsp,
  server: Server
) => {
  const bd = dspBuildDir(dsp, server)
  await step('cmake', ['--build', bd])
  await dspClangdStep(dsp, server)
}

const dspClangdStep = async (
  dsp   : Dsp,
  server: Server
) => {
  const bd = dspBuildDir(dsp, server)
  const cc = 'compile_commands.json'

  if (existsSync(cc))
    await rm(cc, { force: true })

  await copyFile(join(bd, cc), cc)
}

const viewBuildStep = async () => {
  const config = {
    entrypoints: [join(viewSrcDir, 'index.html')],
    outdir     : viewBuildDir,
    minify     : true ,
    splitting  : false,
    define     : {
      'process.env.NODE_ENV': JSON.stringify('production')
    },
    plugins    : [
      tailwind
    ]
  }

  await rm(viewBuildDir, { recursive: true, force: true })

  try {
    await Bun.build(config)
  }
  catch (error) {
    await rm(viewBuildDir, { recursive: true, force: true })
    throw error
  }
}

const viewServeStep = async (view: View) => {
  const indexDir  = join('..', view === 'hot' ? viewSrcDir   : viewBuildDir)
  const staticDir =            view === 'hot' ? viewAssetDir : viewBuildDir
  const config    = {
    port       : viewServePort,
    routes     : Object.assign({
      '/' : (await import(join(indexDir, 'index.html'))).default,
      '/*': (req: Request) => {
        const name = req.url.substring(req.headers.get('referer')!.length)
        return new Response(Bun.file(join(staticDir, name)))
      }
    }),
    development: {
      hmr    : true,
      console: true
    },
  }

  const server = Bun.serve(config)
  console.log(`Server is running on ${ server.url }`)
}

const clean = (
  dsp   : Dsp,
  server: Server
) => rm(dspBuildDir(dsp, server), { recursive: true, force: true })

const build = async (
  dsp   : Dsp ,
  view  : View,
  server: Server
) => {
  if (view === 'build')
    await viewBuildStep()

  await dspConfigStep(dsp, server)
  await dspBuildStep (dsp, server)

  if (server === 'live')
    viewServeStep(view)
}

const main = async () => {
  const argv = yargs(hideBin(process.argv)).option({
    'dsp': {
      alias  : 'd',
      choices: DSP_CHOICES
    },
    'view': {
      alias  : 'v',
      choices: VIEW_CHOICES
    },
    'server': {
      alias  : 's',
      choices: SERVER_CHOICES
    },
    'release': {
      alias    : 'r'      ,
      type     : 'boolean',
      conflicts: ['dsp', 'view', 'server']
    },
    'force': {
      alias    : 'f',
      type     : 'boolean'
    }
  })
  .parseSync()

  if (argv.release) {
    argv.dsp    = 'Release'
    argv.view   = 'build'
    argv.server = 'bundle'
  }
  else {
    if (argv.dsp === undefined)
      argv.dsp = 'Debug'

    if (argv.server === undefined)
      argv.server = 'live'

    if (argv.view === undefined)
      argv.view = argv.server === 'live' ? 'hot' : 'build'

    if (argv.server === 'bundle' && argv.view === 'hot')
      throw new Error('\'view\' must be \'build\' when \'server\' is \'bundle\'')
  }

  if (argv.force)
    await clean(
      argv.dsp,
      argv.server)

  await build(
    argv.dsp ,
    argv.view,
    argv.server)
}

if (import.meta.main)
  await main()
