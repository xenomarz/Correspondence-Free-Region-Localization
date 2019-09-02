import {
    CHANGE_SPLIT_ORIENTATION,
    CHANGE_MODEL_VIEWPORT_COLOR,
    CHANGE_SOUP_VIEWPORT_COLOR,
    CHANGE_MODEL_COLOR,
    CHANGE_SOUP_COLOR,
    CHANGE_WIREFRAME_VISIBILITY,
    CHANGE_MODEL_VIEW_VISIBILITY,
    CHANGE_SOUP_VIEW_VISIBILITY,
    CHANGE_DELTA,
    CHANGE_LAMBDA,
    CHANGE_SEAMLESS_WEIGHT,
    CHANGE_POSITION_WEIGHT,
    CHANGE_GRID_HORIZONTAL_COLOR,
    CHANGE_GRID_VERTICAL_COLOR,
    CHANGE_GRID_BACKGROUND_COLOR1,
    CHANGE_GRID_BACKGROUND_COLOR2,
    CHANGE_HIGHLIGHTED_FACE_COLOR,
    CHANGE_DRAGGED_FACE_COLOR,
    CHANGE_FIXED_FACE_COLOR,
    CHANGE_VERTEX_ENERGY_COLOR,
    CHANGE_VERTEX_ENERGY_TYPE,
    CHANGE_GRID_SIZE,
    CHANGE_GRID_TEXTURE_SIZE,
    CHANGE_GRID_LINE_WIDTH,
    CHANGE_UNIT_GRID_VISIBILITY,
    CHANGE_SOUP_VIEW_GRID_TEXTURE_VISIBILITY,
    CHANGE_OPTIMIZATION_DATA_MONITOR_VISIBILITY,
    CHANGE_SOLVER_STATE,
    CHANGE_MODEL_FILENAME
} from './actions.js';

export const SplitOrientation = {
    HORIZONTAL: 'Horizontal',
    VERTICAL: 'Vertical'
};

export const Visibility = {
    VISIBLE: 'Visible',
    HIDDEN: 'Hidden'
};

export const EnergyType = {
    SEPERATION: 'Seperation',
    SEAMLESS: 'Seamless'
};

export const SolverState = {
    ON: 'On',
    OFF: 'Off'
};

export const isVisible = (visibility) => {
    return visibility === Visibility.VISIBLE;
};

export const isSolverOn = (solverState) => {
    return solverState === SolverState.ON;
};

export const  visibilityFromBool = (bool) => {
    return bool ? Visibility.VISIBLE : Visibility.HIDDEN;
};

export const  SolverStateFromBool = (bool) => {
    return bool ? SolverState.ON : SolverState.OFF;
};

export const solverStateText = (solverState) => {
    return solverState;
};

export const energyTypeText = (energyType) => {
    return energyType;
};

export const splitOrientationText = (splitOrientation) => {
    return splitOrientation;
};

const INITIAL_STATE = {
    splitOrientation: SplitOrientation.HORIZONTAL,
    modelViewportColor: 'rgb(200,200,200)',
    soupViewportColor: 'rgb(200,200,200)',
    modelColor: 'rgb(255, 255, 255)',
    soupColor: 'rgb(255, 255, 255)',
    wireframeVisibility: Visibility.VISIBLE,
    modelViewVisibility: Visibility.VISIBLE,
    soupViewVisibility: Visibility.VISIBLE,
    delta: 0.9,
    lambda: 0.1,
    seamlessWeight: 0,
    positionWeight: 0,
    gridHorizontalColor: 'rgb(0,0,0)',
    gridVerticalColor: 'rgb(0,0,0)',
    gridBackgroundColor1: 'rgb(0,0,200)',
    gridBackgroundColor2: 'rgb(200,200,0)',
    highlightedFaceColor: 'rgb(0,255,0)',
    draggedFaceColor: 'rgb(0,0,255)',
    fixedFaceColor: 'rgb(255,0,0)',
    vertexEnergyColor: 'rgb(255,0,0)',
    vertexEnergyType: EnergyType.SEPERATION,
    gridSize: 3,
    gridTextureSize: 8,
    gridLineWidth: 0,
    unitGridVisibility: Visibility.HIDDEN,
    soupViewGridTextureVisibility: Visibility.HIDDEN,
    optimizationDataMonitorVisibility: Visibility.VISIBLE,
    solverState: SolverState.OFF,
    modelFilename: ''
};

export const reducer = (state = INITIAL_STATE, action) => {
    switch (action.type) {
        case CHANGE_SPLIT_ORIENTATION:
            return {
                ...state,
                splitOrientation: action.orientation
            };
        case CHANGE_MODEL_VIEWPORT_COLOR:
            return {
                ...state,
                modelViewportColor: action.color
            };
        case CHANGE_SOUP_VIEWPORT_COLOR:
            return {
                ...state,
                soupViewportColor: action.color
            };
        case CHANGE_MODEL_COLOR:
            return {
                ...state,
                modelColor: action.color
            };
        case CHANGE_SOUP_COLOR:
            return {
                ...state,
                soupColor: action.color
            };
        case CHANGE_WIREFRAME_VISIBILITY:
            return {
                ...state,
                wireframeVisibility: action.visibility
            };
        case CHANGE_MODEL_VIEW_VISIBILITY:
            return {
                ...state,
                modelViewVisibility: action.visibility
            };
        case CHANGE_SOUP_VIEW_VISIBILITY:
            return {
                ...state,
                soupViewVisibility: action.visibility
            };
        case CHANGE_DELTA:
            return {
                ...state,
                delta: action.delta
            };
        case CHANGE_LAMBDA:
            return {
                ...state,
                lambda: action.lambda
            };
        case CHANGE_SEAMLESS_WEIGHT:
            return {
                ...state,
                seamlessWeight: action.weight
            };
        case CHANGE_POSITION_WEIGHT:
            return {
                ...state,
                postionWeight: action.weight
            };
        case CHANGE_GRID_HORIZONTAL_COLOR:
            return {
                ...state,
                gridHorizontalColor: action.color
            };
        case CHANGE_GRID_VERTICAL_COLOR:
            return {
                ...state,
                gridVerticalColor: action.color
            };
        case CHANGE_GRID_BACKGROUND_COLOR1:
            return {
                ...state,
                gridBackgroundColor1: action.color
            };
        case CHANGE_GRID_BACKGROUND_COLOR2:
            return {
                ...state,
                gridBackgroundColor2: action.color
            };
        case CHANGE_HIGHLIGHTED_FACE_COLOR:
            return {
                ...state,
                highlightedFaceColor: action.color
            };
        case CHANGE_DRAGGED_FACE_COLOR:
            return {
                ...state,
                draggedFaceColor: action.color
            };
        case CHANGE_FIXED_FACE_COLOR:
            return {
                ...state,
                fixedFaceColor: action.color
            };
        case CHANGE_VERTEX_ENERGY_COLOR:
            return {
                ...state,
                vertexEnergyColor: action.color
            };
        case CHANGE_VERTEX_ENERGY_TYPE:
            return {
                ...state,
                vertexEnergyType: action.type
            };
        case CHANGE_GRID_SIZE:
            return {
                ...state,
                gridSize: action.size
            };
        case CHANGE_GRID_TEXTURE_SIZE:
            return {
                ...state,
                gridTextureSize: action.size
            };

        case CHANGE_GRID_LINE_WIDTH:
            return {
                ...state,
                gridLineWidth: action.width
            };            
        case CHANGE_UNIT_GRID_VISIBILITY:
            return {
                ...state,
                unitGridVisibility: action.visibility
            };    
        case CHANGE_SOUP_VIEW_GRID_TEXTURE_VISIBILITY:
            return {
                ...state,
                soupViewGridTextureVisibility: action.visibility
            }; 
        case CHANGE_OPTIMIZATION_DATA_MONITOR_VISIBILITY:
            return {
                ...state,
                optimizationDataMonitorVisibility: action.visibility
            }; 
        case CHANGE_SOLVER_STATE:
            return {
                ...state,
                solverState: action.state
            };
        case CHANGE_MODEL_FILENAME:
            return {
                ...state,
                modelFilename: action.filename
            };            
        default:
            return state;
    }
};