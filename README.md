# Donation Monitor

Little CLI application that helps you monitor and manage donations you make.

# Usage
## Adding new projects/payments
`donate add <project>`  
Adds empty project.

`donate add <project> <payment> [date]`  
Adds payment to the project at date. If no date is specified current date is used. If project of given name doesn't exist, it will be automatically created. 

## Showing donations
`donate [show] [filters]`  
Shows donations devided by month and project

## Detailed info about project
`donate project <project>`  
Shows all payments from given project

## Showing projects
`donate projects [filters]`  
Shows all projects matching filters

## Removing projects
`donate remove <project>`

## Removing payments (not implemented)
`donate remove <filters>`

## Money
`<value>[currency]`  
Examples: 5, 15USD, 3.52$  
Note there is no space between value and currency.

## Filters
`names:<project[,project[,...]]>`  
`min:<money>`  
`max:<money>`  
`from:<date>`  
`to:<date>`  
`between:<date>:<date>`
