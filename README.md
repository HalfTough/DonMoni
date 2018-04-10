# MonEx

Little CLI application that helps you monitor and manage your expenses.

# Usage
## Adding new projects/payments
`monex add <project>`  
Adds empty project.

`monex add <project> <amount> [date]`  
Adds payment to the project at date. If no date is specified current date is used. If project of given name doesn't exist, it will be automatically created. 

## Showing table
`monex [show] [filters]`  
Shows payments devided by month and name

## Detailed info about project
`monex project <project>`  
Shows all payments from given project

## Showing projects
`monex projects [filters]`  
Shows all projects matching filters

## Removing projects
`monex remove <project>`

## Removing payments
`monex remove <filters>`

## Renaming projects
`monex rename <oldname> <newname>`

## Modifying projects
`monex modify <filters> [amount] [date]

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
`on:<date>`
Newer filter overrides an older one so  
`monex show names:project from:today form:2017-01-01`  
will show payments from project starting at 2017-01-01 and from:today will be ignored

## Dates
`YYYY-MM-DD`  
`MM-DD`  
`tomorrow`  
`yesterday`  
`today`

## Recurring payments
You can add recurring payments, by putting `recur:<time>` somwhere after `add`  
`monex add OSM 20$ recur:monthly`  
will add 20$ to project OSM each month, starting today.  
`monex add Krita 5 05-02 recur:2weeks`  
will add 5 of your default currency to project Krita every two weeks, statring on may 02.  

Recurring time can be more complicated if you need it to be
`monex add Wikipedia 10$ recur:1month,week`
