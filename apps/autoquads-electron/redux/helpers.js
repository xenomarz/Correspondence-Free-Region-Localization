import * as EnumsExports from './enums.js';

export const isVisible = (visibility) => {
    return visibility === EnumsExports.Visibility.VISIBLE;
};

export const isSolverOn = (solverState) => {
    return solverState === EnumsExports.SolverState.ON;
};

export const isShapeLoaded = (shapeState) => {
    return shapeState === EnumsExports.LoadState.LOADED;
};

export const isPartialLoaded = (partialState) => {
    return partialState === EnumsExports.LoadState.LOADED;
};

export const isModuleLoaded = (moduleState) => {
    return moduleState === EnumsExports.LoadState.LOADED;
};

export const  visibilityFromBool = (bool) => {
    return bool ? EnumsExports.Visibility.VISIBLE : EnumsExports.Visibility.HIDDEN;
};

export const  SolverStateFromBool = (bool) => {
    return bool ? EnumsExports.SolverState.ON : EnumsExports.SolverState.OFF;
};

export const solverStateText = (solverState) => {
    return solverState;
};

export const splitOrientationText = (splitOrientation) => {
    return splitOrientation;
};

export const isAutocutsEnabled = (algorithmType) => {
    return algorithmType === EnumsExports.AlgorithmType.AUTOCUTS;
};

export const isAutoquadsEnabled = (algorithmType) => {
    return algorithmType === EnumsExports.AlgorithmType.AUTOQUADS;
};