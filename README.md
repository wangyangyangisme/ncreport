NCReport Version 0.7.30 (Release Frozen 2007/06/)
http://ncreport.sourceforge.net

Copyright (c) 2002-2007 Norbert Szabo <nszabo@helta.hu>


For licensing and warranty information, read COPYING.
For installation instructions, read INSTALL.
For what changes have been made, read CHANGES.
For information on what features are planned or unfinished, read TODO.
For general information, read this file (README):


1. What is NCReport?
====================

NCReport is lightweight, fast, easy to use SQL report engine written in C++ 
based on Qt toolkit. Report definition using XML format - stored as file or 
in sql database. Generated document ready to print or (fast) preview.
The XML report definition file is possible to be created and manage 
"by hand" with text editor, but there is a GUI report designer application
already for design report files visually. It's called NCReport-Designer.

About report definition XML file:

This version able to handle only metric measurement. The position and size
values are both in millimeter. Proceeds a detailed documentation about
the features in XML definition and how-to-s, but now you can learn it from 
sample report files ( in /samples)

2. Features
===========

* Connection to SQL databases
* Using text file as datasource
* One "master" query and unlimited "one-row" queries
* Text label, field, lines, rectangle, ellipse, image
* Dynamic lenghted richtext field handling
* Page header/footer
* Data grouping with totals, average
* Group header/footer
* Run query at group footer
* Images (static from NCReport dir.)
* Evaluate "print when" expression
* Parameters
* Lookup class for row-level functions
* Format numeric datas
* Direct output to printer or to PDF file
* Fast preview form


3. Updates & More Information
=============================

Visit: http://ncreport.sourceforge.net


4. Support
==========

Email: <nszabo@helta.hu>

If you have any questions about compiling, installing or using NCReport-Designer,
don't be afraid to contact us using the above email address. Commercial licence holder 
customers gets support.

5. Commercial usage
===================

In order you want to use NCReport and NCReport Designer in your commercial software or
software not relased under the GPL please contact me.
Than you can buy a supported commercial licence at fair price. We can also provide
for official invoice. See prices at the project's website.

6. Feedback
===========

Please send bug reports and feature requests at http://ncreport.sourceforge.net
Don't hesitate to report bugs nor hesitate to send us your wishes - it provides valuable 
feedback that will help to improve future versions of this software.


