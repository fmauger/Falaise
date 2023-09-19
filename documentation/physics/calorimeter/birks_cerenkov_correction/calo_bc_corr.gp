# Ref: NemoDocDB-doc-4412 by E.Chauveau
keV = 1.0

# x :  energy
_bc_corr(x) = 1.001960 * (1.08996 - (1.561100 / ((x / keV)**0.41)));

max(x,y) = x > y ? x : y
bc_corr(x) = max(0.0, _bc_corr(x))

Emin = 0.0 * keV
Emax = 820.0 * keV
Emax = 1200.0 * keV
Emax = 2000.0 * keV

set title "Electron Birks-Cerenkov correction factor\nE_{ref}=1 MeV"
set grid
set samples 4000
set xlabel "Energy deposit (keV)"
set ylabel "Birks-Cerenkov correction factor"
set xrange [Emin/keV:Emax/keV]
set yrange [0.0:1.1]
plot bc_corr(x * keV) notitle with lines lw 2
pause -1

set terminal push
set terminal pdfcairo
set output "calo_birks_cerenkov_correction_factor.pdf"
replot
set output
set terminal pop

bc_energy(x) = x *  bc_corr(x)
set xlabel "Energy deposit (keV)"
set ylabel "Corrected energy (keV)"
set xrange [Emin/keV:Emax/keV]
set yrange [Emin/keV:Emax/keV]
set size ratio -1

set title "Electron Birks-Cerenkov corrected energy\nE_{ref}=1 MeV"
id(x)=x
plot bc_energy(x * keV) notitle with lines lw 2, \
     id(x) notitle lw -1
pause -1

set terminal push
set terminal pdfcairo
set output "calo_birks_cerenkov_corrected_energy.pdf"
replot
set output
set terminal pop

# end
