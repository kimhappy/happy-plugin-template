import { ComboStore, SliderStore, ToggleStore } from "happy-juce"

export const comboProps = (state: ComboStore) => ({
  choices         : state.choices,
  selected        : state.index  ,
  onSelectedChange: state.setIndex
})

export const sliderProps = (state: SliderStore) => ({
  min          : 0                 ,
  max          : 1                 ,
  step         : state.interval    ,
  value        : [state.normalised],
  onValueChange: (value: number[]) => {
    if (value.length === 1)
      state.setNormalised(value[ 0 ])
  }
})

export const switchProps = (state: ToggleStore) => ({
  checked        : state.checked,
  onCheckedChange: state.setChecked
})
