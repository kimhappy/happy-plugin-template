import * as juce from 'happy-juce'
import {
  Table      ,
  TableBody  ,
  TableCell  ,
  TableHead  ,
  TableHeader,
  TableRow
} from '@/component/ui/table'
import logo from '#/logo.png'

export const Info = () => (
  <div className = 'flex items-center'>
    <img src = { logo } width = { 96 } height = { 96 } alt = 'logo'/>
    <Table>
      <TableHeader>
        <TableRow>
          <TableHead>Property</TableHead>
          <TableHead>Value</TableHead>
        </TableRow>
      </TableHeader>
      <TableBody>
        {
          Object.entries(juce.INIT_DATA).map(([key, value], index) => (
            <TableRow key = { index }>
              <TableCell>{ key   }</TableCell>
              <TableCell>{ value }</TableCell>
            </TableRow>
          ))
        }
      </TableBody>
    </Table>
  </div>
)
