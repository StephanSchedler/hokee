# hokee - Automated Housekeeping Book

``hokee`` is a account book software that parses CSV account transaction data to automatically categorize all your income and expenses.

![Summary Page](https://repository-images.githubusercontent.com/270410626/7366c480-cb92-11ea-98b1-908f059a0013)

The hokee budget book is perfect for everyone who wants to have a summary over income and expenses without time-consuming manual accounting. Simply download CSV account transaction data from you online bank account, create simple rules for unkown items, done.

## Online Demo Report

Try the link below, to view an exemplary report generated for some simple dummy data.

> Note that this is a **static** demo, i.e. you cannot modify rules or data.

[Static Online Demo](http://stephanschedler.github.io/hokee)

## OS Support

Operating System | State
-- | --
Windows 10 | stable
Ubuntu 18.04 | unstable (untested)
MAC | unstable (untested)

## Quick Start

- Download [latest release](https://github.com/StephanSchedler/hokee/releases) 
- Extract binaries 
  - Windows:  [7zip](https://www.7-zip.org/download.html)
  - Ubuntu/Mac:  "``tar -xf hokee_os.tar``"
- Start ``bin/hokee``
  - Copy sample data
  - Try to fix all warnings and errors

## How to configure your own CSV data
- Download CSV account transaction data from your online bank account and copy CSV file(s) to a new directory in the input folder
- Copy format.ini from sample input directory to the new folder and adjust the content to the format of your downloaded CSV file(s). 
  > This is the hardest part, I know. However, it is not that hard, and you have to do it just once. Note also, that there are some more format samples in hokee's testdata.
- Delete the sample data in the input folder
- Delete all existing rules
- (Re-)start ``bin/hokee``

### Normal Mode

### Command Line Switches

### Support

## Configure Hokee

### General Settings

### CSV Formats

## Further Infos

### 3rdParty Dependencies 

Project | Type |Link | Description
--- | --- | --- | ---
fmtlib | C++ library | https://github.com/fmtlib/fmt.git | 
cpp-httplib | C++ library | https://github.com/yhirose/cpp-httplib.git |
small-n-flat | images | https://github.com/paomedia/small-n-flat | 