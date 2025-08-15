import { useSliderStore, useToggleStore, useComboStore } from 'happy-juce'
import { Separator                                     } from '@/component/ui/separator'
import { Label                                         } from '@/component/ui/label'
import { Slider                                        } from '@/component/ui/slider'
import { Switch                                        } from '@/component/ui/switch'
import { Combo                                         } from '@/component/ui/combo'
import { sliderProps, switchProps, comboProps          } from '@/util/props'

export const Controls = () => {
  const gain       = useSliderStore('GAIN'           )
  const bypass     = useToggleStore('BYPASS'         )
  const distortion = useComboStore ('DISTORTION_TYPE')

  const gainProps       = sliderProps(gain)
  const bypassProps     = switchProps(bypass)
  const distortionProps = comboProps(distortion)

  return (
    <div className = 'flex flex-col gap-4'>
      <div className = 'flex items-center h-8'>
        <Label     className = 'w-20 justify-center'>Gain</Label>
        <Separator className = 'me-4' orientation = 'vertical'/>
        <Slider    className = 'flex-1' { ...gainProps }/>
        <Separator className = 'ms-4' orientation = 'vertical'/>
        <Label     className = 'w-20 justify-center'> { gain.scaled.toFixed(2) } </Label>
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
