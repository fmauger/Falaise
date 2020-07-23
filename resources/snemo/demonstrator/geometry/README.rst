====================================================================
SuperNEMO Demonstrator Geometry and Vertex Generation Configuration
====================================================================

Configuration files
===================

Description of configuration files and their dependencies:

* ``GeometryVariantRepository.conf``   :  Definition   of  a   variant
  repository for  *geometry* support  only.  This  mechanism describes
  the  various options  (*variants*) selectable  while setting  up the
  demonstrator  geometry and  allows  to describe  use choice  through
  specific *variant profile* files.

  - ``GeometryVariants.conf`` : Configuration  of the geometry variant
    registry
  - ``variants/*.def``  : Definitions  of variant  models and  variant
    parameters
    
* ``GeometryManager.conf`` : Geometry manager's main configuration

  - ``GeometryModels/Demonstrator.conf``  : List  of geometry  model
    definition files for the SN demonstrator module
      
    + ``GeometryModels/*/*.geom`` :  Geometry model  definitions for
      the SN demonstrator module in different geometry variants
	
  - ``GeometryPlugins/*Plugin.conf``   :  Definitions   of  geometry
    plugins (materials support, locators, mag. field)
  - ``GeomIDMaps/*_categories.lis``   :   Definitions  of   geometry
    categories (for geometry mapping)

* ``GeometryService.conf`` : Geometry service's main configuration  
* ``VertexGeneratorVariantRepository.conf`` : Definition  of a variant
  repository  for both  *geometry* and  *vertex generation*  supports.
  Some  *vertex  generators*  options  depend  on  *geometry*  options
  selected by the user.

  - Use the *geometry* variant definitions.
  - ``VertexGeneratorVariants.conf`` :  Configuration of  the geometry
    variant registry
  - ``variants/vertex/*.conf``  : Definitions  of  variant models  and
    variant parameters
  - ``VertexGeneratorVariantsGDM.conf``  : Description  of the  global
    dependency model  for variants  related to vertex  generation with
    respect to geometry options
    
* ``VertexGeneratorManager.conf``  : Vertex  generator manager's  main
  configuration

  - ``VertexModels/*.conf`` : Definitions of  vertex generators for SN
    demonstrator module in different geometry variants

    
Running the Geomtools inspector
================================

Example on Linux:

1. Falaise must be installed and setup because the Geomtools inspector
   needs to  load dynamically the  Falaise shared library  to activate
   specific built objects in it.
2. Cd in the ``resources`` Falaise source directory.
3. Run the Geomtools inspector:
   
   .. code:: bash

      $ _falaise_prefix="`dirname $(which fltags)`/.."
      $ _falaise_resources="$(pwd)"
      $ bxgeomtools_inspector \
	     --logging "trace" \
	     --datatools::logging "trace" \
	     --datatools::resource-path "falaise@${_falaise_resources}" \
	     --variant-config "@falaise:snemo/demonstrator/geometry/GeometryVariantRepository.conf" \
	     --variant-gui \
	     --load-dll "Falaise@${_falaise_prefix}/lib" \
	     --manager-config "@falaise:snemo/demonstrator/geometry/GeometryManager.conf"
..

   **Notes:**

   - the ``--datatools::resource-path`` option registers the Falaise's
     resource directory as the ``"falaise"`` tag,
   - the ``--variant-config``  option defines  the variant  system for
     the SuperNEMO geometry,
   - the ``--variant-gui`` option launches a GUI for selecting variant
     parameters from the geometry variant registry,
   - the ``--load-dll``  option dynamically  loads the  Falaise shared
     library from the proper location,
   - the ``--manager-config`` option loads the main configuration file
     for the SuperNEMO geometry manager.

4. Select the source layout at inspector startup : ``"Basic"`` or ``"RealSource"``
5. Enter the inspector session:

   .. code:: bash
   
      geomtools> help
      ...
      geomtools> list_of_models
      ...
      geomtools> list_of_gids --with-category source_submodule
      List of available GIDs : 
      [1100:0] as 'source_submodule'        
      geomtools> display -yz [1100:0]
      geomtools> display -yz --no-title --output "snemo-basic_source_foils-1.jpeg" [1100:0]
      ...
   ..

   or:

   .. code:: bash
      
      geomtools> display -yz [1100:0]
      geomtools> display -yz --no-title --output "snemo-realistic_flat_source_foils-1.jpeg" [1100:0]
      ...
   ..

6. Quit the session:

   .. code:: bash
      
      geomtools> quit
   ..

   
Running the Genvtx production
================================

1. Falaise must be  installed and setup because  the Genvtx production
   program needs  to load  dynamically the  Falaise shared  library to
   activate specific built objects in it.
2. Cd in the ``resources`` Falaise source directory.
3. Run the Genvtx production to list available vertex generators:

   On  the variant  GUI panel,  please select  the ``"RealisticFlat"``
   source layout in the *Demonstrator  geometry* panel then select the
   ``"snemo_source_strip_23_pad_3_bulk"``  generator  in  the  *Vertex
   generation* panel.
   
   .. code:: bash

      $ _falaise_prefix="`dirname $(which fltags)`/.."
      $ _falaise_resources="$(pwd)"
      $ bxgenvtx_production \
	     --logging "trace" \
	     --datatools::logging "trace" \
	     --datatools::resource-path "falaise@${_falaise_resources}" \
	     --load-dll "Falaise@${_falaise_prefix}/lib" \
	     --variant-config "@falaise:snemo/demonstrator/geometry/VertexGeneratorVariantRepository.conf" \
	     --variant-gui \
	     --variant-store "/tmp/_falaise_genvtx.profile" \
	     --geometry-manager "@falaise:snemo/demonstrator/geometry/GeometryManager.conf" \
	     --vertex-generator-manager "@falaise:snemo/demonstrator/geometry/VertexGeneratorManager.conf" \
	     --list
      $ cat /tmp/_falaise_genvtx.profile
      #@format=datatools::configuration::variant
      #@format.version=1.0
      #@organization=snemo
      #@application=falaise

      [registry="geometry"]
      layout = "Basic"
      layout/if_basic/magnetic_field = true
      layout/if_basic/magnetic_field/is_active/type = "UniformVertical"
      layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/magnitude = 25 gauss
      layout/if_basic/magnetic_field/is_active/type/if_uniform_vertical/direction = "+z"
      layout/if_basic/source_layout = "RealisticFlat"
      layout/if_basic/source_calibration = false
      layout/if_basic/shielding = true
      calo_film_thickness = 25 um

      [registry="vertexes"]
      generator = "snemo_source_strip_23_pad_3_bulk"
      
   ..
   
   

4. Run  the Genvtx  production  to shoot  vertexes  from the  selected
   specific  generator  (loading   the  previously  generated  variant
   profile):
   
   .. code:: bash

      $ bxgenvtx_production \
	     --logging="trace" \
	     --datatools::logging="trace" \
	     --datatools::resource-path="falaise@${_falaise_resources}" \
	     --load-dll="Falaise@${_falaise_prefix}/lib" \
	     --variant-config="@falaise:snemo/demonstrator/geometry/VertexGeneratorVariantRepository.conf" \
	     --variant-load="/tmp/_falaise_genvtx.profile" \
	     --geometry-manager="@falaise:snemo/demonstrator/geometry/GeometryManager.conf" \
	     --vertex-generator-manager="@falaise:snemo/demonstrator/geometry/VertexGeneratorManager.conf" \
	     --shoot \
	     --prng-seed=314159 \
	     --number-of-vertices=10000 \
	     --vertex-modulo=1000 \
	     --visu \
	     --visu-view="yz" \
	     --visu-max-counts=10000 \
	     --visu-spot-zoom=4 \
	     --visu-spot-color="red" \
	     --visu-object="[1100:0]" \
	     --visu-output \
	     --visu-output-file="/tmp/_falaise_genvtx-dd.xml.gz" 
      $ less /tmp/_falaise_genvtx-dd.xml.gz
      $ bxgeomtools_inspector \
	     --datatools::resource-path "falaise@${_falaise_resources}" \
	     --variant-config "@falaise:snemo/demonstrator/geometry/GeometryVariantRepository.conf" \
	     --variant-load "/tmp/_falaise_genvtx.profile" \
	     --load-dll "Falaise@${_falaise_prefix}/lib" \
	     --manager-config "@falaise:snemo/demonstrator/geometry/GeometryManager.conf" \
	     --visu-object "[1100:0]"
      geomtools> load_display_data --name "vertexes" --input "/tmp/_falaise_genvtx-dd.xml.gz"
      geomtools> display -yz --with-display-data [1100:0]
      geomtools> display -yz --no-title --with-display-data --output "snemo-realistic_flat_source_foils-vertex_23_3.jpeg" [1100:0]
      geomtools> quit
   ..	     
	     
.. end
