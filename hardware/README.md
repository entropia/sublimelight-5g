# sublimeilght-5g hardware

## known issues/gotchas

The pins in the footprint of the 3.3V regulator U5 are counted in the other direction compared to the 78-style regulators it replaces, but I used the 78-style kicad footprint there.
My solution was to replace the angeled pin header soldered to the module with a straight pin connector and then soldering U5 to the board so it points to the side of the PCB.
This also solves the possible issue that the levers of J8 aren't that far away from U5 when U5 is mounted at a 90 degree angle.
