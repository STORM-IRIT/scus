%  Make the scatteres for a simulation and store
%  it in a file for later simulation use

%   Joergen Arendt Jensen, Feb. 26, 1997

function mk_pht()
    [phantom_positions, phantom_amplitudes] = scus_phantom();
    
    save pht_data.mat phantom_positions phantom_amplitudes
