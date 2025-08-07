import { create } from 'zustand'

export type CntStore = {
  value: number

  increment: () => void
}

export const useCntStore = create< CntStore >((set, get) => {
  return {
    value: 0,

    increment: () => set({ value: get().value + 1 })
  }
})
