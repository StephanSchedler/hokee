# 1. hokee - Automated Housekeeping Book

``hokee`` is a account book software that parses CSV account transaction data to automatically categorize all your income and expenses.

![Summary Page](https://repository-images.githubusercontent.com/270410626/55b97b80-21d2-11eb-9593-a0704b888264)

The hokee budget book is perfect for everyone who wants to have a summary over income and expenses without time-consuming manual accounting. Simply download CSV account transaction data from you online bank account, create simple rules for unkown items, done.

- [1. hokee - Automated Housekeeping Book](#1-hokee---automated-housekeeping-book)
  - [1.1. Online Demo](#11-online-demo)
  - [1.2. Quick Start](#12-quick-start)
  - [1.3. How to start with your own CSV data](#13-how-to-start-with-your-own-csv-data)
    - [1.3.1. CSV Format Description](#131-csv-format-description)
    - [1.3.2. General Settings](#132-general-settings)
  - [1.4. Support](#14-support)
  - [1.5. Commandline Switches](#15-commandline-switches)
  - [1.6. Supported Operating Systems](#16-supported-operating-systems)
  - [1.7. 3rdParty Dependencies](#17-3rdparty-dependencies)

## 1.1. Online Demo

Try the link below, to view an exemplary report generated for some test data.

> Note that this is a **static** demo, i.e. you cannot modify rules or data.

[Static Online Demo](http://stephanschedler.github.io/hokee)

## 1.2. Quick Start

- Download [latest release](https://github.com/StephanSchedler/hokee/releases) 
- Extract binaries 
  - Windows:  [7zip](https://www.7-zip.org/download.html)
  - Ubuntu/Mac:  "``tar -xf hokee_os.tar``"
- Start ``bin/hokee[.exe]``
  - Click the copy icon, to copy test data to the input folder
  - Try to fix all warnings and errors in the test data

## 1.3. How to start with your own CSV data

- Download CSV account transaction data from your online bank account and copy CSV file(s) to a new directory in the input folder (blue folder icon in the navigation bar)
- Copy ``format.ini`` from input directory ``ABC`` to the new folder and adjust the content to the format of your downloaded CSV file(s). 
  > This is the hardest part, I know. However, it is not as hard as you might think, and you have to do it just once. See also ``CSV Format Description`` section below.
- Delete the sample directory ``ABC`` in the input folder
- Delete all existing rules. (Open a rule and click red delete icon.)
- (Re-)start ``bin/hokee[.exe]``

### 1.3.1. CSV Format Description

For the ``hokee`` to be able to understand your CSV data, you have to create a ``format.ini`` and copy into your the input folder of your CSV data.

Format option | Description
-- | --
``AccountOwner`` | Name of account owner as it is used in the CSV data. (Only used if you have separate ``Payer`` and ``Payee`` columns to merge both fields to single ``PayerPayee`` field. See hint below)
``HasHeader`` | Indicates whether the CSV data contains a header with a column name. (``true`` or ``false``)
``ColumnNames`` | Semicolon separated list of column names as used in the CSV data. (Only used, if ``HasHeader`` is ``true``)
``IgnoreLines`` | Number of lines in CSV data that are skipped by the parser 
``HasDoubleQuotes`` | Indicates whether the column data is encapusalted in ``"double quotes"``. (``true`` or ``false``)
``HasTrailingDelimiter`` | Indicates whether CSV data is completed with an empty column (``true`` or ``false``)
``Delimiter`` | Column delimiter character (e.g. ``';'``, ``','``, ``':'``, ...);
``DateFormat`` | Format of the date column (e.g. ``dd.mm.yyyy``, ``dd.mm.yy``, ...)
``PayerPayee`` | Index (>= 0) of the ``PayerPayee`` column in the CSV data. (``-1`` if there are separated ``Payer`` and ``Payee`` columns) 
``Payer`` | Index (>= 0) of the ``Payer`` column in the CSV data. (``-1`` if there is a single ``PayerPayee`` column) 
``Payee`` | Index (>= 0) of the ``Payee`` column in the CSV data. (``-1`` if there is a single ``PayerPayee`` column)
``Description`` | Index (>= 0) of the ``Description`` column in the CSV data. (``-1`` if there is no such column)
``Type`` | Index (>= 0) of the ``Type`` column in the CSV data. (``-1`` if there is no such column)
``Date`` | Index (>= 0) of the ``Date`` column in the CSV data. (``-1`` if there is no such column. See also ``DateFormat`` option.)
``Account`` | Index (>= 0) of the ``Account`` column in the CSV data. (``-1`` if there is no such column)
``Value`` | Index (>= 0) of the ``Value`` column in the CSV data. (``-1`` if there is no such column)

> **Hint:** <br>
> - Your CSV file may contain a single ``PayerPayee`` column, or separate ``Payer`` and ``Payee`` columns. If you have separate columns, either ``Payer`` or ``Payee`` should always be the ``AccountOwner`` and ``hokee`` will always try to combine data into a single ``PayerPayee`` field.
> - Have a look into the test data folder of ``hokee``. There are samples for different format options.

### 1.3.2. General Settings

``hokee`` does save settings in a settings file. Default location is ``$HOME/hokee/hokee.ini``. (Can be changed with commandline arguments.)

Setting | Description
-- | --
``InputDirectory`` | Path to the input directory for csv data. ``hokee`` searches subfolders for ``*.csv``, parses according to the ``format.ini`` file in that subdirectory and assigns items to categories according to the ``RuleSetFile``. (relative to ``hokee.ini`` settings file.)
``RuleSetFile`` | Path to the file that defines the rules used to assign items to categories. (relative to ``hokee.ini`` settings file.)
``Browser`` | Commandlin of a browser, such as ``firefox``, ``chrome`` or ``edge``. (``hokee`` does use a browser to show a html report. This setting defines the commandline used to start the browser. The correct ``url`` will be appended by ``hokee``) 
``Explorer`` | This setting is used by ``hokee`` if you click to blue folder icon to open the ``InputDirectory`` or a parent folder of a file.

## 1.4. Support

If you need help or found a bug, click the support icon in the naviation bar to generate a email.

> **Note**: This email does contain an anonymized version of your CSV data. If you do not want to submit that data, delete it manually before submitting the email.

## 1.5. Commandline Switches
```
Usage: hokee [options] [path]

  path            - Path to config file.
                    (default: $HOME/hokee/hokee.ini)
  -v,--verbose    - Print trace infos to console.
  --version       - Show version infos.
  -h,--help       - Show this help.
  -s,--support    - Generate anonymized support information.
```

## 1.6. Supported Operating Systems

Operating System | State
-- | --
Windows 10 | stable
Ubuntu 18.04 | **unstable**
Mac | **unstable**

## 1.7. 3rdParty Dependencies 

Project | Type |Link | Description
--- | --- | --- | ---
fmtlib | C++ library | https://github.com/fmtlib/fmt.git | 
cpp-httplib | C++ library | https://github.com/yhirose/cpp-httplib.git |
small-n-flat | images | https://github.com/paomedia/small-n-flat | 