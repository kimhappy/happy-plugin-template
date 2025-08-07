import { create } from 'zustand'

export type MeterStore = {
  outputLevels: number[]

  setOutputLevels: (dbLevels: number[]) => void
}

export const useMeterStore = create< MeterStore >((set) => {
  return {
    outputLevels: [],

    setOutputLevels: (dbLevels: number[]) => set({ outputLevels: dbLevels })
  }
})
