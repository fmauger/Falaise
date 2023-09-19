
keV = 1.0
MeV = 1000 * keV

aq_p0 = 77.4
aq_p1 = 0.639
aq_p2 = 2.34

# x : energy
mod_energy(x) = 1.0 / (aq_p1 * (x / MeV) + 1.0);
aqf(x) = (-aq_p0 * (mod_energy(x)**aq_p2 - mod_energy(x)**(aq_p2/2)))

Emin = 0.0 * MeV
Emax = 10.0 * MeV

set grid
set samples 4000
set xlabel "Energy deposit (MeV)"
set ylabel "Alpha quenching factor"
plot [Emin/MeV:Emax/MeV] aqf(x * MeV) notitle with lines lw 2
pause -1

# end
