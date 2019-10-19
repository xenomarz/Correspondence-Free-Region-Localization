import * as EnumsExports from './enums.js';

export const isVisible = (visibility) => {
    return visibility === EnumsExports.Visibility.VISIBLE;
};

export const isSolverOn = (solverState) => {
    return solverState === EnumsExports.SolverState.ON;
};

export const isModelLoaded = (modelState) => {
    return modelState === EnumsExports.LoadState.LOADED;
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