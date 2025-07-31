# Online documents support by Doxygen

The generated online documentation is located in Docs/Doxygen/html, with index.html as the entry point. By default, the configuration is set to
- EVE_GRAPHICS_BT820
- FT4222_PLATFORM


## How to rebuild the documents according to required configurations
1. Install Doxygen from https://www.doxygen.nl/download.html
2. Download source code and Doxygen configuration  (current repo) 
3. Launch the Windows command prompt and go to the directory ${repo_path}/EveApps-BT82X/Docs/Doxygen
4. Open configuration file 'Doxyfile', change the configuration **PREDEFINED**
```
    eg. PREDEFINED             = EVE_GRAPHICS_BT820 \
                                 MPSSE_PLATFORM \
                                 _WIN32
```
5. The graphs are created by DOT (available at https://graphviz.org/). If you prefer not to include the graphs, set **HAVE_DOT** to NO. If you wish to keep it, make sure to update **DOT_PATH** to the directory where Graphviz is installed on your system.
6. Run Doxygen to generate the updated documentation. The new files will be created in the *html* folder.
```
    eg. ${doxygen_path}/doxygen.exe Doxyfile
```
