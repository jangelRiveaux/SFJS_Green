//
//  DecoderIntervalManager.h
//  LRKGA
//
//  Created by Jose Angel Riveaux on 14/01/24.
//

#ifndef DecoderIntervalManager_h
#define DecoderIntervalManager_h


class DecoderIntervalManager{
    
protected:
    float  stepSize;

public:
    DecoderIntervalManager(float stepSize):stepSize(stepSize){};
    
    /**
     * given a chromosome key and the probabilityKey of a chromosome return if the value is in the valid interval of search of the current cromosome
     */
    bool isKeyInInterval(double key, double chromosomeProbabilityKey) const{
        return abs(key - chromosomeProbabilityKey) < stepSize;
    };
    
    /**
     * given a chromosome key and the probabilityKey of a chromosome return if the value is in the valid interval of search of the current cromosome
     */
    double keysDistances(double key, double chromosomeProbabilityKey) const{
        return abs(key - chromosomeProbabilityKey);
    };
};
#endif /* DecoderIntervalManager_h */
