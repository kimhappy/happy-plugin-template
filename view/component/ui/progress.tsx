import * as ProgressPrimitive from '@radix-ui/react-progress'
import { cn }                 from '@/util/cn'

export type ProgressMarker = {
  values  : number[]
} | {
  interval: number
  include : number
}

export type ProgressProps = {
  value     : number
  min       : number
  max       : number
  marker   ?: ProgressMarker & {
    lineClassName ?: string
    labelClassName?: string
    format?        : (value: number) => string
  }
  className?: string
}

const getMarkers = (
  marker: ProgressMarker,
  min   : number        ,
  max   : number
) => {
  if ('values' in marker)
    return marker.values.toSorted()

  const interval = marker.interval
  const include  = marker.include
  const markers  = [include]

  for (let i = 1; include + i * interval <= max; ++i)
    markers.push(include + i * interval)

  for (let i = 1; include - i * interval >= min; ++i)
    markers.unshift(include - i * interval)

  return markers
}

const percentage = (
  value: number,
  min  : number,
  max  : number
) => {
  const clamped = Math.max(Math.min(value, max), min)
  return (clamped - min) * 100 / (max - min)
}

export const Progress = (props: ProgressProps) => {
  const ratio   = percentage(props.value, props.min, props.max)
  const markers = props.marker ? getMarkers(props.marker, props.min, props.max) : []
  const hasMarkers = markers.length > 0

  return (
    <div className = { cn('relative w-full', hasMarkers && 'pb-6', props.className) }>
      <ProgressPrimitive.Root
        data-slot = 'progress'
        value     = { ratio }
        className = 'bg-primary/10 relative h-2 w-full overflow-hidden rounded-full'
      >
        <ProgressPrimitive.Indicator
          data-slot = 'progress-indicator'
          className = 'bg-primary h-full w-full flex-1 transition-all'
          style     = { { transform: `translateX(-${ 100 - ratio }%)` } }
        />

        { markers.map((markerValue, index) => {
          const markerRatio = percentage(markerValue, props.min, props.max)

          return (
            <div
              key       = { index }
              className = { cn(
                'absolute top-1/2 -translate-y-1/2 w-0.25 h-1/2 bg-foreground/20',
                props.marker?.lineClassName
              ) }
              style     = { { left: `${ markerRatio }%` } }
            />
          )
        }) }
      </ProgressPrimitive.Root>

      { markers.map((markerValue, index) => {
        const markerRatio = percentage(markerValue, props.min, props.max)

        return (
          <div
            key       = { index }
            className = { cn(
              'absolute mt-0.5 -translate-x-1/2 text-xs text-foreground/60',
              props.marker?.labelClassName
            ) }
            style     = { { left: `${ markerRatio }%` } }
          >
            { props.marker?.format ? props.marker.format(markerValue) : markerValue }
          </div>
        )
      }) }
    </div>
  )
}
