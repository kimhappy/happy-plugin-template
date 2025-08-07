import { Label         } from '@/component/ui/label'
import { Progress      } from '@/component/ui/progress'
import { useMeterStore } from '@/store/meter'

export const Meter = () => {
  const { outputLevels } = useMeterStore()

  return (
    <div className = 'flex flex-col gap-3'>
      <Label className = 'justify-center'>Output Level</Label>
      { outputLevels.map((value, index) => (
        <Progress
          key       = { index }
          value     = { value }
          min       = { -75   }
          max       = {  15   }
          marker    = { {
            interval: 10,
            include :  0
          } }
          className = 'h-3'
        />
      )) }
    </div>
  )
}
