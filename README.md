# AGKI_Abschlussprojekt


REMEMBER DRAW BY REPETITION SO WE DON'T RUN FOREVER!!!


Debugging Ergebnisse 17/01:
-deltaInputWeights im ersten backpropStep ist ziemlich groß (vermutlich wegen summierung) und dadurch werden die nosigNeurons im zweiten backpropStep wahnsinnig groß (negativ groß), wodurch ihr sigmoid extrem nah an null ist (~10^-300)
