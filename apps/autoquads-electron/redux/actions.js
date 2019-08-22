import nanoid from '../web_modules/nano-id.js';
export const CHANGE_SPLIT_ORIENTATION = 'CHANGE_SPLIT_ORIENTATION';
export const CHANGE_MODEL_VIEWPORT_COLOR = 'CHANGE_MODEL_VIEWPORT_COLOR';
export const CHANGE_SUOP_VIEWPORT_COLOR = 'CHANGE_SUOP_VIEWPORT_COLOR';
export const CHANGE_MODEL_COLOR = 'CHANGE_MODEL_COLOR';
export const CHANGE_SUOP_COLOR = 'CHANGE_SUOP_COLOR';
export const CHANGE_WIREFRAME_VISIBILITY = 'CHANGE_WIREFRAME_VISIBILITY';
export const CHANGE_MODEL_VIEW_VISIBILITY = 'CHANGE_MODEL_VIEW_VISIBILITY';
export const CHANGE_SUOP_VIEW_VISIBILITY = 'CHANGE_SUOP_VIEW_VISIBILITY';
export const CHANGE_DELTA = 'CHANGE_DELTA';
export const CHANGE_LAMBDA = 'CHANGE_LAMBDA';
export const CHANGE_SEAMLESS_WEIGHT = 'CHANGE_SEAMLESS_WEIGHT';
export const CHANGE_POSITION_WEIGHT = 'CHANGE_POSITION_WEIGHT';
export const CHANGE_GRID_HORIZONTAL_COLOR = 'CHANGE_GRID_HORIZONTAL_COLOR';
export const CHANGE_GRID_VERTICAL_COLOR = 'CHANGE_GRID_VERTICAL_COLOR';
export const CHANGE_GRID_BACKGROUND_COLOR1 = 'CHANGE_GRID_BACKGROUND_COLOR1';
export const CHANGE_GRID_BACKGROUND_COLOR2 = 'CHANGE_GRID_BACKGROUND_COLOR2';
export const CHANGE_HIGHLIGHTED_FACE_COLOR = 'CHANGE_HIGHLIGHTED_FACE_COLOR';
export const CHANGE_DRAGGED_FACE_COLOR = 'CHANGE_DRAGGED_FACE_COLOR';
export const CHANGE_FIXED_FACE_COLOR = 'CHANGE_FIXED_FACE_COLOR';
export const CHANGE_VERTEX_ENERGY_COLOR = 'CHANGE_VERTEX_ENERGY_COLOR';
export const CHANGE_VERTEX_ENERGY_TYPE = 'CHANGE_VERTEX_ENERGY_TYPE';
export const CHANGE_GRID_SIZE = 'CHANGE_GRID_SIZE';
export const CHANGE_GRID_TEXTURE_SIZE = 'CHANGE_GRID_TEXTURE_SIZE';
export const CHANGE_GRID_LINE_WIDTH = 'CHANGE_GRID_LINE_WIDTH';
export const CHANGE_UNIT_GRID_VISIBILITY = 'CHANGE_UNIT_GRID_VISIBILITY';
export const CHANGE_SUOP_VIEW_GRID_TEXTURE_VISIBILITY = 'CHANGE_SUOP_VIEW_GRID_TEXTURE_VISIBILITY';
export const CHANGE_OPTIMIZATION_DATA_MONITOR_VISIBILITY = 'CHANGE_OPTIMIZATION_DATA_MONITOR_VISIBILITY';
export const CHANGE_SOLVER_STATE = 'CHANGE_SOLVER_STATE';
export const CHANGE_MODEL_FILENAME = 'CHANGE_MODEL_FILENAME';

export const changeSplitOrientation = orientation => {
    return {
        type: CHANGE_SPLIT_ORIENTATION,
        orientation: orientation
    };
};

export const changeModelViewportColor = color => {
    return {
        type: CHANGE_MODEL_VIEWPORT_COLOR,
        color: color
    };
};

export const changeSuopViewportColor = color => {
    return {
        type: CHANGE_SUOP_VIEWPORT_COLOR,
        color: color
    };
};

export const changeModelColor = color => {
    return {
        type: CHANGE_MODEL_COLOR,
        color: color
    };
};

export const changeSuopColor = color => {
    return {
        type: CHANGE_SUOP_COLOR,
        color: color
    };
};

export const changeWireframeVisibility = visibility => {
    return {
        type: CHANGE_WIREFRAME_VISIBILITY,
        visibility: visibility
    };
};

export const changeModelViewVisibility = visibility => {
    return {
        type: CHANGE_Model_VIEW_VISIBILITY,
        visibility: visibility
    };
};

export const changeSuopViewVisibility = visibility => {
    return {
        type: CHANGE_SUOP_VIEW_VISIBILITY,
        visibility: visibility
    };
};

export const changeDelta = delta => {
    return {
        type: CHANGE_DELTA,
        delta: delta
    };
};

export const changeLambda = lambda => {
    return {
        type: CHANGE_LAMBDA,
        lambda: lambda
    };
};

export const changeSeamlessWeight = weight => {
    return {
        type: CHANGE_SEAMLESS_WEIGHT,
        weight: weight
    };
};

export const changePositionWeight = weight => {
    return {
        type: CHANGE_POSITION_WEIGHT,
        weight: weight
    };
};

export const changeGridHorizontalColor = color => {
    return {
        type: CHANGE_GRID_HORIZONTAL_COLOR,
        color: color
    };
};

export const changeGridVerticalColor = color => {
    return {
        type: CHANGE_GRID_VERTICAL_COLOR,
        color: color
    };
};

export const changeGridBackgroundColor1 = color => {
    return {
        type: CHANGE_GRID_BACKGROUND_COLOR1,
        color: color
    };
};

export const changeGridBackgroundColor2 = color => {
    return {
        type: CHANGE_GRID_BACKGROUND_COLOR2,
        color: color
    };
};

export const changeHighlightedFaceColor = color => {
    return {
        type: CHANGE_HIGHLIGHTED_FACE_COLOR,
        color: color
    };
};

export const changeDraggedFaceColor = color => {
    return {
        type: CHANGE_DRAGGED_FACE_COLOR,
        color: color
    };
};

export const changeFixedFaceColor = color => {
    return {
        type: CHANGE_FIXED_FACE_COLOR,
        color: color
    };
};

export const changeVertexEnergyColor = color => {
    return {
        type: CHANGE_VERTEX_ENERGY_COLOR,
        color: color
    };
};

export const changeVertexEnergyType = type => {
    return {
        type: CHANGE_VERTEX_ENERGY_TYPE,
        type: type
    };
};

export const changeGridSize = size => {
    return {
        type: CHANGE_GRID_SIZE,
        size: size
    };
};

export const changeGridTextureSize = size => {
    return {
        type: CHANGE_GRID_TEXTURE_SIZE,
        size: size
    };
};

export const changeGridLineWidth = width => {
    return {
        type: CHANGE_GRID_LINE_WIDTH,
        width: width
    };
};

export const changeUnitGridVisibility = visibility => {
    return {
        type: CHANGE_UNIT_GRID_VISIBILITY,
        visibility: visibility
    };
};

export const changeSuopViewGridTextureVisibility = visibility => {
    return {
        type: CHANGE_SUOP_VIEW_GRID_TEXTURE_VISIBILITY,
        visibility: visibility
    };
};

export const changeOptimizationDataMonitorVisibility = visibility => {
    return {
        type: CHANGE_OPTIMIZATION_DATA_MONITOR_VISIBILITY,
        visibility: visibility
    };
};

export const changeSolverState = state => {
    return {
        type: CHANGE_SOLVER_STATE,
        state: state
    };
};

export const changeModelFilename = filename => {
    return {
        type: CHANGE_MODEL_FILENAME,
        filename: filename
    };
};