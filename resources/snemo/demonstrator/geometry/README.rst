=========================================================
SuperNEMO Demonstrator Geometry Configuration
=========================================================

Running the Geomtools inspector
================================

Example on Linux:

1. Falaise must be installed and setup because the Geomtools inspector needs to load dynamically
   the Falaise shared library to activate specific built objects in it.
2. Cd in the ``resources/snemo/demonstrator/geometry`` Falaise source directory:
3. Run the Geomtools inspector:
   
   .. code:: bash

      $ _falaise_prefix="`dirname $(which fltags)`/.."
      $ _falaise_resources="$(pwd)"
      $ bxgeomtools_inspector \
	     --datatools::logging="trace" \
	     --datatools::resource-path="falaise@${_falaise_resources}" \
	     --variant-config "@falaise:snemo/demonstrator/geometry/GeometryVariantRepository.conf" \
	     --variant-gui \
	     -G "trace" \
	     --load-dll "Falaise@${_falaise_prefix}/lib" \
	     --manager-config "@falaise:snemo/demonstrator/geometry/GeometryManager.conf"
..

   **Notes:**

   - the  ``--datatools::resource-path`` option registers the Falaise's resource directory as the ``"falaise"`` tag,
   - the  ``--variant-config`` option defines the variant system for the SuperNEMO geometry,
   - the  ``--variant-gui`` option launches a GUI for selecting variant parameters from the geometry variant registry,
   - the  ``--load-dll`` option dynamically loads the Falaise shared library from the proper location,
   - the  ``--manager-config`` option loads the main configuration file for the SuperNEMO geometry manager.

4. Select the source layout at inspector startup : ``"Basic"`` or ``"RealSource"``
5. Enter the inspector session

   .. code:: bash
   
      geomtools> help
      ...
      geomtools> list_of_models
      ...
      geomtools> d -yz --output "snemo-basic_source_foils-1.jpeg" source_submodule.model
      ...
   ..

   or:

   .. code:: bash
      
      geomtools> d -yz --output "snemo-real_source_foils-1.jpeg" source_submodule.model
      ...
   ..

6. Quit the session:

   .. code:: bash
      
      geomtools> quit
   ..

.. end
