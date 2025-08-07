# Happy Plugin Template

<div align = 'center'>

![screenshot](asset/screenshot.png)

![C++23](https://img.shields.io/badge/C%2B%2B23-659AD2?style=for-the-badge)
![CMake](https://img.shields.io/badge/cmake-008FBA?style=for-the-badge)
![JUCE](https://img.shields.io/badge/JUCE-8DC63F?style=for-the-badge)
![TypeScript](https://img.shields.io/badge/typescript-007ACC.svg?style=for-the-badge)
![Bun](https://img.shields.io/badge/bun-FFF4E4.svg?style=for-the-badge)
![React](https://img.shields.io/badge/react-409CB4.svg?style=for-the-badge)
![Zustand](https://img.shields.io/badge/zustand-7B4019?style=for-the-badge)
![TailwindCSS](https://img.shields.io/badge/tailwindcss-38B2AC.svg?style=for-the-badge)
![shadcn/ui](https://img.shields.io/badge/shadcn/ui-000000.svg?style=for-the-badge)

</div>

**JUCE WebView** plugin project template.

## Build
```sh
bun b
```

| Option | Choices | Default | Description |
| - | - | - | - |
| `-d` | `Debug`, `Release` | `Debug` | C++ build mode. |
| `-s` | `live`, `bundle` | `live` | Whether to bundle WebView files into the binary or use live server. |
| `-v` | `hot`, `build` | `hot` if `-s` is `live`, `build` if `-s` is `bundle` | Whether to build WebView files or enable hot reloading. Cannot be `hot` when `-s` is `bundle`. |
| `-r` | | | Same as `-d release -s bundle -v build`. |
| `-f` | | | Remove CMake configuration directory in `/build` before building. |

> If you run `bun b` with the `-s live` option, the local server (http://localhost:3000) will be started.
> To run the app built with the `-s live` option, the local server must be running.
> You can specify the port number using the `HAPPY_PORT` environment variable.

## Add shadcn/ui components
```sh
bunx shadcn@latest add ...
```

## Clean up
```sh
bun z
```

## Related Links
- [happy-juce](https://github.com/kimhappy/happy-juce)
