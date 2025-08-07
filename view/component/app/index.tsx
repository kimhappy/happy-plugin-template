import * as juce         from 'happy-juce'
import { useEffect     } from 'react'
import { Info          } from './info'
import { Buttons       } from './buttons'
import { Controls      } from './controls'
import { Meter         } from './meter'
import { Separator     } from '@/component/ui/separator'
import { useMeterStore } from '@/store/meter'

export const App = () => {
  const { setOutputLevels } = useMeterStore()

  useEffect(() => {
    const handleExampleEvent = (objectFromBackend: any) => {
      console.log('handleExampleEvent:', objectFromBackend)
    }

    const handleOutputLevel = async () => {
      const resource = await juce.fetchResource('outputLevel.json')

      if (resource) {
        const levelData = await resource.json() as { levels: number[] }
        setOutputLevels(levelData.levels)
      }
    }

    const exampleEventId = juce.addEventReceiver('exampleEvent', handleExampleEvent)
    const outputLevelId  = juce.addEventReceiver('outputLevel' , handleOutputLevel )

    return () => {
      juce.removeEventReceiver(exampleEventId)
      juce.removeEventReceiver(outputLevelId )
    }
  }, [])

  return (
    <div className = 'flex flex-col gap-4 p-4 justify-center min-h-screen'>
      <Info/>
      <Separator/>
      <Buttons/>
      <Separator/>
      <Controls/>
      <Separator/>
      <Meter/>
    </div>
  )
}
