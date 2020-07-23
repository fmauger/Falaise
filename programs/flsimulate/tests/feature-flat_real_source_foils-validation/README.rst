=====================================================================================
Test Falaise flat realistic source foil geometry and associated vertex generators
=====================================================================================
  
Running the simulation configuration with Falaise:
======================================================

#. Edit the flsimulate variant configuration file ``flsimulate.profile``:

   .. code:: bash

      $ cat flsimulate.profile 
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
      generator = "real_flat_source_full_foils_se82_bulk"

      [registry="primary_events"]
      generator = "Se82.2nubb"

      [registry="simulation"]
      physics_mode = "Constructors"
      physics_mode/if_constructors/em_model = "standard"
      production_cuts = true
      output_profile = "all_details"

   ..

#. Edit the flsimulate configuration file ``simu.conf``:
   
   .. code:: bash

      $ cat simu.conf
      #@description flsimulate configuration script 
      #@key_label  "name"
      #@meta_label "type"
      [name="flsimulate" type="flsimulate::section"]
      numberOfEvents : integer = 10
      [name="flsimulate.simulation" type="flsimulate::section"]
      rngEventGeneratorSeed         : integer = 1
      rngVertexGeneratorSeed        : integer = 2
      rngGeant4GeneratorSeed        : integer = 3
      rngHitProcessingGeneratorSeed : integer = 4
      [name="flsimulate.variantService" type="flsimulate::section"]
      profile : string as path = "flsimulate.profile"

   ..

#. Run flsimulate:
   
   .. code:: bash

      $ flsimulate \
	  --config "simu.conf" \
	  --output-metadata-file "flsimulate_meta.xml" \
 	  --output-file "flsimulate.xml.gz" 
   ..

   
#. Visualize:
   
   .. code:: bash

      $ flvisualize \
          --variant-profile "flsimulate.profile" \
 	  --input-file "flsimulate.xml.gz" 
   ..

  
Validation script
======================================================
   
.. code:: bash

   $ 

..
    
.. end
   
