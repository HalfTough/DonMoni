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

## Config
MonEx will read config from `/etc/xdg/monex.conf` and `/home/gurtos/.config/monex.conf`. The letter will override the firs one. On top of that, you can pass some of the settings as arguments

Example [config file](https://github.com/HalfTough/MonEx/blob/master/config/monex.conf).

### List of settings
| in file | as argument | values |
|---|---|---|
|language| - | language in "en_US" format |
|profile|--profile, -p| string |
|default_currency|--currency| string |
|compare_money|--compare| `ignore``convert` |
|print_money|--print| `mixed` `convert` |
|exchange_server| - | url |
|exchange_time| - | integer > 0 |
|row_coloring| - | see [#coloring](coloring) |
|min_uncut_cols|--min_uncut_cols | integer >= 0 |
|show_to|--show_to| `last` `today` `force_today` |
|default_timeframe|--timeframe, -t| `year` `month` `week` `day` |
|time_inverval|--interval| interes > 0 |
|week_start|--week_start| `monday` `tuesday` `...` `mon` `tue` `...` 0<integer<=7 |
|time_shift|--shift| `start` `end` integer |
|symbol_separator| - | `space` `none` string |
|currency_separator| - | `comma` `none` `space` string |

###coloring
You can change look of the output using `row_coloring` setting.  
You can use instructions:
| attribute |
|---|
| bold |
| dim |
| italic |
| underline |
| blink |
| invert |
| hidden |

You can color using terminal defaults
| black | red | green | yellow | blue | magenta | cyan | gray |
For 256 colors use
| color255 |
and replace 255 with number of color

For true color use
| color#ffffff |
and replace #ffffff with your color

To set background color use
| back:<color> |
