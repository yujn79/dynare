/*
 * Copyright (C) 2003-2009 Dynare Team
 *
 * This file is part of Dynare.
 *
 * Dynare is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Dynare is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Dynare.  If not, see <http://www.gnu.org/licenses/>.
 */

using namespace std;

#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdlib>
#include <cstring>
#ifndef PACKAGE_VERSION
  #define PACKAGE_VERSION 4.
#endif
#include "macro/MacroDriver.hh"

/* Prototype for second part of main function
   Splitting main() in two parts was necessary because ParsingDriver.h and MacroDriver.h can't be
   included simultaneously (because of Bison limitations).
*/
void main2(stringstream &in, string &basename, bool debug, bool clear_all, bool no_tmp_terms, bool warn_uninit);

void
usage()
{
  cerr << "Dynare usage: dynare mod_file [debug] [noclearall] [savemacro[=macro_file]] [onlymacro] [nolinemacro] [notmpterms] [warn_uninit]" << endl;
  exit(EXIT_FAILURE);
}

int
main(int argc, char** argv)
{
  if (argc < 2)
    {
      cerr << "Missing model file!" << endl;
      usage();
    }

  bool clear_all = true;
  bool save_macro = false;
  string save_macro_file;
  bool debug = false;
  bool no_tmp_terms = false;
  bool only_macro = false;
  bool no_line_macro = false;
  bool warn_uninit = false;

  // Parse options
  for (int arg = 2; arg < argc; arg++)
    {
      if (!strcmp(argv[arg], "debug"))
        debug = true;
      else if (!strcmp(argv[arg], "noclearall"))
        clear_all = false;
      else if (!strcmp(argv[arg], "onlymacro"))
        only_macro = true;
      else if (strlen(argv[arg]) >= 9 && !strncmp(argv[arg], "savemacro", 9))
        {
          save_macro = true;
          if (strlen(argv[arg]) > 9)
            {
              if (strlen(argv[arg]) == 10 || argv[arg][9] != '=')
                {
                  cerr << "Incorrect syntax for savemacro option" << endl;
                  usage();
                }
              save_macro_file = string(argv[arg] + 10);
            }
        }
      else if (!strcmp(argv[arg], "nolinemacro"))
        no_line_macro = true;
      else if (!strcmp(argv[arg], "notmpterms"))
        no_tmp_terms = true;
      else if (!strcmp(argv[arg], "warn_uninit"))
        warn_uninit = true;
      else
        {
          cerr << "Unknown option: " << argv[arg] << endl;
          usage();
        }
    }

  cout << "Starting Dynare (version " << PACKAGE_VERSION << ")." << endl
       << "Starting preprocessing of the model file ..." << endl;

  // Construct basename (i.e. remove file extension if there is one)
  string basename = argv[1];
  size_t pos = basename.find_last_of('.');
  if (pos != string::npos)
    basename.erase(pos);

  // Do macro processing
  MacroDriver m;

  stringstream macro_output;
  m.parse(argv[1], macro_output, debug, no_line_macro);
  if (save_macro)
    {
      if (save_macro_file.empty())
        save_macro_file = basename + "-macroexp.mod";
      ofstream macro_output_file(save_macro_file.c_str());
      if (macro_output_file.fail())
        {
          cerr << "Cannot open " << save_macro_file << " for macro output" << endl;
          exit(EXIT_FAILURE);
        }
      macro_output_file << macro_output.str();
      macro_output_file.close();
    }

  if (only_macro)
    return EXIT_SUCCESS;

  // Do the rest
  main2(macro_output, basename, debug, clear_all, no_tmp_terms, warn_uninit);

  return EXIT_SUCCESS;
}
