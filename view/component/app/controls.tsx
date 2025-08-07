import * as juce     from 'happy-juce'
import { Separator } from '@/component/ui/separator'
import { Label     } from '@/component/ui/label'
import { Slider    } from '@/component/ui/slider'
import { Switch    } from '@/component/ui/switch'
import { Combo     } from '@/component/ui/combo'

export const Controls = () => {
  const {
    sliderProps: gainProps,
    scaled     : gainScaled
  } = juce.useSliderStore('GAIN')

  const {
    switchProps: bypassProps,
  } = juce.useToggleStore('BYPASS')

  const {
    comboProps: distortionProps,
  } = juce.useComboStore('DISTORTION_TYPE')

  return (
    <div className = 'flex flex-col gap-4'>
      <div className = 'flex items-center h-8'>
        <Label     className = 'w-20 justify-center'>Gain</Label>
        <Separator className = 'me-4' orientation = 'vertical'/>
        <Slider    className = 'flex-1' { ...gainProps }/>
        <Separator className = 'ms-4' orientation = 'vertical'/>
        <Label     className = 'w-20 justify-center'> { gainScaled.toFixed(2) } </Label>
      </div>

      <div className = 'flex items-center h-8'>
        <Label     className = 'w-20 justify-center'>Bypass</Label>
        <Separator className = 'me-4' orientation = 'vertical'/>
        <Switch    { ...bypassProps }/>
      </div>

      <div className = 'flex items-center h-8'>
        <Label     className = 'w-20 justify-center'>Distortion</Label>
        <Separator className = 'me-4' orientation = 'vertical'/>
        <Combo     name      = 'distortion' { ...distortionProps }/>
      </div>
    </div>
  )
}
