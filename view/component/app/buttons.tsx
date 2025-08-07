import * as juce       from 'happy-juce'
import { Button      } from '@/component/ui/button'
import { useCntStore } from '@/store/cnt'

export const Buttons = () => {
  const { value, increment } = useCntStore()

  const handleNativeFunctionCall = async () => {
    const result = await juce.callNativeFunction('nativeFunction', 'one', 2, null)
    console.log('handleNativeFunctionCall:', result)
  }

  const handleEmitEvent = () => {
    increment()

    juce.sendEvent('exampleJsEvent', {
      emittedCount: value
    })
  }

  return (
    <div className = 'flex items-center justify-around'>
      <Button variant = 'outline' onClick = { handleNativeFunctionCall }>
        Call a C++ function
      </Button>

      <Button variant = 'outline' onClick = { handleEmitEvent }>
        Emit a frontend event { value }
      </Button>
    </div>
  )
}
