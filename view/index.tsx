import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import { App        } from '@/component/app'
import                     '@/index.css'

const createHotRoot = (elem: HTMLElement) => {
  import.meta.hot.data.root ??= createRoot(elem)
  import.meta.hot.accept()
  return import.meta.hot.data.root
}

const elem = document.getElementById('root')!
const root = (import.meta.hot ? createHotRoot : createRoot)(elem)

root.render(
  <StrictMode>
    <App/>
  </StrictMode>)
