# MonEx

Little CLI application that helps you monitor and manage your expenses.

# Installation
1. Download or clone this project  
`git clone https://github.com/HalfTough/MonEx`
2. `cd MonEx`
3. `mkdir build`
4. `cd build`
5. `cmake ..`  
6. `make`
7. `sudo make install`

# Basic Usage
Let's say you want to keep a track of how much you spend on everything that's unhealthy. You bought package of cigarettes for 10$ and cola for 1$.  
`monex add cigarettes 10$`  
if "$" is yous default currency, you can skip it  
`monex add cola 1`

Now running `monex` will show a little table

```
Name        April Total  
cigarettes    10$   10$  
cola           1$    1$  
Total         11$   11$
```

If you want to add a payment for a date different than today, you can do it by adding date after amount  
`monex add cigarettes 10 2018-03-25`

Running `monex` without arguments will now show us
```
Name        March April Total
cigarettes    10$   10$   20$
cola           0$    1$    1$
Total         10$   11$   21$
```

## Adding new categories/payments
`monex add <name>`  
Adds empty category.

`monex add <name> <amount> [date]`  
Adds payment to the category `name` at `date`. If no date is specified current date is used. If category of given name doesn't exist, it will be automatically created. 

## Showing table
`monex [show] [filters]`  

More about [filters](#filters)

By default your payments will be sorted by month. You can change it to year, week or a day by either setting `default_timeframe` in your [config](#config) or putting `--timeframe` as an argument.  
`monex --timeframe week`  
will return  
```
Name        19.03-25.03 26.03-01.04 02.04-08.04 09.04-15.04 Total
cigarettes          10$          0$          0$         10$   20$
cola                 0$          0$          0$          1$    1$
Total               10$          0$          0$         11$   21$
```

if you want time frame to be 5 days use  
`monex --timeframe day --interval 5`  
or set `default_timeframe=day` and `time_interval=5` in your config.

### Money
`<value>[currency]`  
Examples: 5, 15USD, 3.52$  
Note there is no space between value and currency.

ISO codes will be converted into matching currency symbol.

You can mix different currencies  
`monex add figures 50$,1200JPY`  
will show as  
```
Name          April      Total
figures  1200￥, 50$ 1200￥, 50$
```

You can also use custom currencies  
`monex add myDoge 12DogeCoins`


### Conversion
By default MonEx won't convert between different currencies. If you put both euros and dollars into your table. Both will be displayed.

Same goes for filtering using `min:` or `max:`. Given amount of money A will only be considered to be greater or equal than B if each currency in B is present in A and none of them is smaller.

We can change this behavior and convert everything to our default currency.  
Printing can be changed by setting `print_money` to `convert` instead of `mixed`. And comparison by setting `convert` to `compare_money`.

If you do that, you also need to set your `exchange_server` and possibly `api_key` if your server demands it.
MonEx uses api from [http://fixer.io](fixer.io). Getting key for the api is free as long you use less than 1000 calls per month (MonEx will only update currencies if they are older than a day) and source of the project is available on github.  
If you want me to add suport for different APIs, let me know via github issues.

Have in mind that if conversion rate is not found, currency will be treated as worthless. 

## Detailed info about category
`monex category <name>`  
Shows all payments from given category

## Showing categories
`monex categories [filters]`  
Shows all projects matching filters

## Removing categories
`monex remove <name>`

## Removing payments
`monex remove <filters>`

## Renaming categories
`monex rename <oldname> <newname>`

## Modifying payments
`monex modify <filters> [amount] [date]`

## Filters
`names:<category[,category[,...]]>` – only show from listed categories  
`min:<money>` – only show payments >= min  
`max:<money>` – only show payments <= max  
`from:<date>` – only show payments made after "date"  
`to:<date>` – only show payments made before "date"  
`between:<date>:<date>` – only show payments made between dates  
`on:<date>` – only show payments made on "date"

Newer filter overrides an older one so  
`monex show names:tickets from:today form:2017-01-01`  
will show payments from category named "tickets" starting at 2017-01-01 and `from:today` will be ignored

## Dates
`YYYY-MM-DD`  
`MM-DD`  
`tomorrow`  
`yesterday`  
`today`

## Profiles
You might not want to keep all of your categories together. If you want some of them to be seperated, you can put them in different profile. You do it by adding `--profile name` or `-p name` to your command.

`monex -p donations add Krita 2$`  
adds 2$ to category Krita in profile donations
`monex -p donations`
shows table for profile donations

You can list all of your profiles  
`monex profiles`

Delete profile
`monex profile remove name`

Rename profile
`monex profile rename name newname`

## Recurring payments
You can add recurring payments, by putting `recur:<time>` somwhere after `add`  
`monex add OSM 20$ recur:monthly`  
will add 20$ to project OSM each month, starting today.  
`monex add Krita 5 05-02 recur:2weeks`  
will add 5 of your default currency to project Krita every two weeks, statring on may 02.  

Recurring time can be more complicated if you need it to be
`monex add Wikipedia 10$ recur:1month,week`

You can also add `until:date` which will stop adding payments at specified point.

## Config
MonEx will read config from `/etc/xdg/monex.conf` and `~/.config/monex.conf`. The letter will override the firs one. On top of that, you can pass some of the settings as arguments

Example [config file](https://github.com/HalfTough/MonEx/blob/master/config/monex.conf).

### List of settings
| in file | as argument | values |
|---|---|---|
|language| - | language in "en_US" format |
|profile|--profile, -p| string |
|default_currency|--currency| string |
|compare_money|--compare| `ignore` `convert` |
|print_money|--print| `mixed` `convert` |
|exchange_server| - | url |
|exchange_time| - | integer > 0 |
|row_coloring| - | see [#coloring](coloring) |
|min_uncut_cols|--min_uncut_cols | integer >= 0 |
|show_to|--show_to| `last` `today` `force_today` |
|default_timeframe|--timeframe, -t| `year` `month` `week` `day` |
|time_inverval|--interval| interes > 0 |
|week_start|--week_start| `monday` `tuesday` `...` `mon` `tue` `...` 1<=integer<=7 |
|time_shift|--shift| `start` `end` integer |
|symbol_separator| - | `space` `none` string |
|currency_separator| - | `comma` `none` `space` string |

### Coloring
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

`black` `red` `green` `yellow` `blue` `magenta` `cyan` `gray`

For 256 colors use

`color255`

and replace 255 with number of color

For true color use

`color#ffffff`

and replace #ffffff with your color

To set background color use

`back:<color>`

Separate attributes by "," and rows by ";"
