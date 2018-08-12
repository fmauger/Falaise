========================================================
Validation of I/O for flsimulate output using Bayeux 3.4 
========================================================

Test/validation material related to issue #115.

The ``flread_sd.cxx`` programs reads a flsimulate output file to check
I/O backward compatibility.

As    Bayeux    3.4   provides    a    new    version   2    of    the
``mctools::base_step_hit`` serializable class, for Falaise linked with
Bayeux >=3.4,  we check the  backward compatibility of  the flsimulate
output  deserialization by  attempting  to loading  the *old*  archive
format from the sample file ``flsim1.xml`` which uses the version 1 of
the  ``mctools::base_step_hit``  class.   The associated  output  file
``out.xml.bz2``          is          compared          to          the
``samples/flread1_out_version2.xml.bz2`` file which corresponds to the
same information stored in ``flsim1.xml``  which uses version 2 of the
``mctools::base_step_hit``.

.. end
