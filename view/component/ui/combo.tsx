import { useState } from 'react'
import { cn       } from '@/util/cn'
import { Button   } from '@/component/ui/button'
import {
  Check,
  ChevronsUpDown
} from 'lucide-react'
import {
  Command     ,
  CommandEmpty,
  CommandGroup,
  CommandInput,
  CommandItem ,
  CommandList
} from '@/component/ui/command'
import {
  Popover       ,
  PopoverContent,
  PopoverTrigger
} from '@/component/ui/popover'

export type ComboProps = {
  name             : string  ,
  choices          : string[],
  selected         : number  ,
  className?       : string  ,
  onSelectedChange?: (value: number) => void
}

export const Combo = ({
  name     ,
  choices  ,
  selected ,
  className,
  onSelectedChange
}: ComboProps) => {
  const [open, setOpen] = useState(false)

  return (
    <Popover open = { open } onOpenChange = { setOpen }>
      <PopoverTrigger asChild>
        <Button
          variant       = 'outline'
          role          = 'combobox'
          aria-expanded = { open }
          className     = { cn('w-[200px] justify-between', className) }
        >
          { choices[ selected ] }
          <ChevronsUpDown className = 'opacity-50'/>
        </Button>
      </PopoverTrigger>
      <PopoverContent className = 'w-[200px] p-0'>
        <Command>
          <CommandInput placeholder = { `Search ${ name }...` } className = 'h-9' />
          <CommandList>
            <CommandEmpty>No { name } found.</CommandEmpty>
            <CommandGroup>
              {
                choices.map((elem, idx) => (
                  <CommandItem
                    key      = { idx            }
                    value    = { idx.toString() }
                    onSelect = { () => {
                      onSelectedChange?.(idx)
                      setOpen (false)
                    } }
                  >
                    { elem }
                    <Check
                      className = { cn(
                        'ml-auto',
                        selected === idx ? 'opacity-100' : 'opacity-0'
                      ) }
                    />
                  </CommandItem>
                ))
              }
            </CommandGroup>
          </CommandList>
        </Command>
      </PopoverContent>
    </Popover>
  )
}
