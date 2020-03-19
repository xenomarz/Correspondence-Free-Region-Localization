import nanoid from '../web_modules/nano-id.js';
import * as Enums from './enums.js';

export const CHANGE_ALGORITHM_TYPE = 'CHANGE_ALGORITHM_TYPE';
export const CHANGE_SPLIT_ORIENTATION = 'CHANGE_SPLIT_ORIENTATION';
export const CHANGE_SHAPE_VIEWPORT_COLOR = 'CHANGE_SHAPE_VIEWPORT_COLOR';
export const CHANGE_PARTIAL_VIEWPORT_COLOR = 'CHANGE_PARTIAL_VIEWPORT_COLOR';
export const CHANGE_SHAPE_COLOR = 'CHANGE_SHAPE_COLOR';
export const CHANGE_PARTIAL_COLOR = 'CHANGE_PARTIAL_COLOR';
export const CHANGE_SHAPE_WIREFRAME_VISIBILITY = 'CHANGE_SHAPE_WIREFRAME_VISIBILITY';
export const CHANGE_PARTIAL_WIREFRAME_VISIBILITY = 'CHANGE_PARTIAL_WIREFRAME_VISIBILITY';
export const CHANGE_SHAPE_FAT_WIREFRAME_VISIBILITY = 'CHANGE_SHAPE_FAT_WIREFRAME_VISIBILITY';
export const CHANGE_PARTIAL_FAT_WIREFRAME_VISIBILITY = 'CHANGE_PARTIAL_FAT_WIREFRAME_VISIBILITY';
export const CHANGE_SHAPE_VIEW_VISIBILITY = 'CHANGE_SHAPE_VIEW_VISIBILITY';
export const CHANGE_PARTIAL_VIEW_VISIBILITY = 'CHANGE_PARTIAL_VIEW_VISIBILITY';
export const CHANGE_AUTOCUTS_WEIGHT = 'CHANGE_AUTOCUTS_WEIGHT';
export const CHANGE_DELTA = 'CHANGE_DELTA';
export const CHANGE_LAMBDA = 'CHANGE_LAMBDA';
export const CHANGE_ZETA = 'CHANGE_ZETA';
export const CHANGE_SEAMLESS_WEIGHT = 'CHANGE_SEAMLESS_WEIGHT';
export const CHANGE_SELECTED_EDGE_SEAMLESS_WEIGHT = 'CHANGE_SELECTED_EDGE_SEAMLESS_WEIGHT';
export const CHANGE_SELECTED_EDGE_SEAMLESS_ANGLE_WEIGHT = 'CHANGE_SELECTED_EDGE_SEAMLESS_ANGLE_WEIGHT';
export const CHANGE_SELECTED_EDGE_SEAMLESS_LENGTH_WEIGHT = 'CHANGE_SELECTED_EDGE_SEAMLESS_LENGTH_WEIGHT';
export const CHANGE_SINGULARITY_WEIGHT = 'CHANGE_SINGULARITY_WEIGHT';
export const CHANGE_SINGULARITY_INTERVAL = 'CHANGE_SINGULARITY_INTERVAL';
export const CHANGE_POSITION_WEIGHT = 'CHANGE_POSITION_WEIGHT';
export const CHANGE_GRID_HORIZONTAL_COLOR = 'CHANGE_GRID_HORIZONTAL_COLOR';
export const CHANGE_GRID_VERTICAL_COLOR = 'CHANGE_GRID_VERTICAL_COLOR';
export const CHANGE_GRID_BACKGROUND_COLOR1 = 'CHANGE_GRID_BACKGROUND_COLOR1';
export const CHANGE_GRID_BACKGROUND_COLOR2 = 'CHANGE_GRID_BACKGROUND_COLOR2';
export const CHANGE_HIGHLIGHTED_FACE_COLOR = 'CHANGE_HIGHLIGHTED_FACE_COLOR';
export const CHANGE_DRAGGED_FACE_COLOR = 'CHANGE_DRAGGED_FACE_COLOR';
export const CHANGE_FIXED_FACE_COLOR = 'CHANGE_FIXED_FACE_COLOR';
export const CHANGE_EDITED_EDGE_COLOR = 'CHANGE_EDITED_EDGE_COLOR';
export const CHANGE_HIGHLIGHTED_EDGE_COLOR = 'CHANGE_HIGHLIGHTED_EDGE_COLOR';
export const CHANGE_GRID_SIZE = 'CHANGE_GRID_SIZE';
export const CHANGE_GRID_TEXTURE_SIZE = 'CHANGE_GRID_TEXTURE_SIZE';
export const CHANGE_GRID_LINE_WIDTH = 'CHANGE_GRID_LINE_WIDTH';
export const CHANGE_UNIT_GRID_VISIBILITY = 'CHANGE_UNIT_GRID_VISIBILITY';
export const CHANGE_PARTIAL_VIEW_GRID_TEXTURE_VISIBILITY = 'CHANGE_PARTIAL_VIEW_GRID_TEXTURE_VISIBILITY';
export const CHANGE_OPTIMIZATION_DATA_MONITOR_VISIBILITY = 'CHANGE_OPTIMIZATION_DATA_MONITOR_VISIBILITY';
export const CHANGE_SOLVER_STATE = 'CHANGE_SOLVER_STATE';
export const CHANGE_SHAPE_FILENAME = 'CHANGE_SHAPE_FILENAME';
export const CHANGE_PARTIAL_FILENAME = 'CHANGE_PARTIAL_FILENAME';
export const CHANGE_MODULE_FILENAME = 'CHANGE_MODULE_FILENAME';
export const CHANGE_SHAPE_STATE = 'CHANGE_SHAPE_STATE';
export const CHANGE_PARTIAL_STATE = 'CHANGE_PARTIAL_STATE';
export const CHANGE_MODULE_STATE = 'CHANGE_MODULE_STATE';
export const CHANGE_OBJECTIVE_FUNCTION_PROPERTY_COLOR = 'CHANGE_OBJECTIVE_FUNCTION_PROPERTY_COLOR';
export const CHANGE_OBJECTIVE_FUNCTION_PROPERTY_VISIBILITY = 'CHANGE_OBJECTIVE_FUNCTION_PROPERTY_VISIBILITY';

export const setAlgorithmType = algorithmType => {
    return {
        type: CHANGE_ALGORITHM_TYPE,
        algorithmType: algorithmType
    };
};

export const setSplitOrientation = orientation => {
    return {
        type: CHANGE_SPLIT_ORIENTATION,
        orientation: orientation
    };
};

export const setShapeViewportColor = color => {
    return {
        type: CHANGE_SHAPE_VIEWPORT_COLOR,
        color: color
    };
};

export const setPartialViewportColor = color => {
    return {
        type: CHANGE_PARTIAL_VIEWPORT_COLOR,
        color: color
    };
};

export const setShapeColor = color => {
    return {
        type: CHANGE_SHAPE_COLOR,
        color: color
    };
};

export const setPartialColor = color => {
    return {
        type: CHANGE_PARTIAL_COLOR,
        color: color
    };
};

export const showShapeWireframe = () => {
    return {
        type: CHANGE_SHAPE_WIREFRAME_VISIBILITY,
        visibility: Enums.Visibility.VISIBLE
    };
};

export const hideShapeWireframe = () => {
    return {
        type: CHANGE_SHAPE_WIREFRAME_VISIBILITY,
        visibility: Enums.Visibility.HIDDEN
    };
};

export const showPartialWireframe = () => {
    return {
        type: CHANGE_PARTIAL_WIREFRAME_VISIBILITY,
        visibility: Enums.Visibility.VISIBLE
    };
};

export const hidePartialWireframe = () => {
    return {
        type: CHANGE_PARTIAL_WIREFRAME_VISIBILITY,
        visibility: Enums.Visibility.HIDDEN
    };
};

export const showShapeFatWireframe = () => {
    return {
        type: CHANGE_SHAPE_FAT_WIREFRAME_VISIBILITY,
        visibility: Enums.Visibility.VISIBLE
    };
};

export const hideShapeFatWireframe = () => {
    return {
        type: CHANGE_SHAPE_FAT_WIREFRAME_VISIBILITY,
        visibility: Enums.Visibility.HIDDEN
    };
};

export const showPartialFatWireframe = () => {
    return {
        type: CHANGE_PARTIAL_FAT_WIREFRAME_VISIBILITY,
        visibility: Enums.Visibility.VISIBLE
    };
};

export const hidePartialFatWireframe = () => {
    return {
        type: CHANGE_PARTIAL_FAT_WIREFRAME_VISIBILITY,
        visibility: Enums.Visibility.HIDDEN
    };
};

export const showShapeView = () => {
    return {
        type: CHANGE_SHAPE_VIEW_VISIBILITY,
        visibility: Enums.Visibility.VISIBLE
    };
};

export const hideShapeView = () => {
    return {
        type: CHANGE_SHAPE_VIEW_VISIBILITY,
        visibility: Enums.Visibility.HIDDEN
    };
};

export const showPartialView = () => {
    return {
        type: CHANGE_PARTIAL_VIEW_VISIBILITY,
        visibility: Enums.Visibility.VISIBLE
    };
};

export const hidePartialView = () => {
    return {
        type: CHANGE_PARTIAL_VIEW_VISIBILITY,
        visibility: Enums.Visibility.HIDDEN
    };
};

export const setAutocutsWeight = weight => {
    return {
        type: CHANGE_AUTOCUTS_WEIGHT,
        weight: weight
    };
};

export const setDelta = delta => {
    return {
        type: CHANGE_DELTA,
        delta: delta
    };
};

export const setLambda = lambda => {
    return {
        type: CHANGE_LAMBDA,
        lambda: lambda
    };
};

export const setZeta = zeta => {
    return {
        type: CHANGE_ZETA,
        zeta: zeta
    };
};

export const setSeamlessWeight = weight => {
    return {
        type: CHANGE_SEAMLESS_WEIGHT,
        weight: weight
    };
};

export const setSelectedEdgeSeamlessAngleWeight = weight => {
    return {
        type: CHANGE_SELECTED_EDGE_SEAMLESS_ANGLE_WEIGHT,
        weight: weight
    };
};

export const setSelectedEdgeSeamlessLengthWeight = weight => {
    return {
        type: CHANGE_SELECTED_EDGE_SEAMLESS_LENGTH_WEIGHT,
        weight: weight
    };
};

export const setSingularityWeight = weight => {
    return {
        type: CHANGE_SINGULARITY_WEIGHT,
        weight: weight
    };
};

export const setSingularityInterval = interval => {
    return {
        type: CHANGE_SINGULARITY_INTERVAL,
        interval: interval
    };
};

export const setPositionWeight = weight => {
    return {
        type: CHANGE_POSITION_WEIGHT,
        weight: weight
    };
};

export const setGridHorizontalColor = color => {
    return {
        type: CHANGE_GRID_HORIZONTAL_COLOR,
        color: color
    };
};

export const setGridVerticalColor = color => {
    return {
        type: CHANGE_GRID_VERTICAL_COLOR,
        color: color
    };
};

export const setGridBackgroundColor1 = color => {
    return {
        type: CHANGE_GRID_BACKGROUND_COLOR1,
        color: color
    };
};

export const setGridBackgroundColor2 = color => {
    return {
        type: CHANGE_GRID_BACKGROUND_COLOR2,
        color: color
    };
};

export const setHighlightedFaceColor = color => {
    return {
        type: CHANGE_HIGHLIGHTED_FACE_COLOR,
        color: color
    };
};

export const setHighlightedEdgeColor = color => {
    return {
        type: CHANGE_HIGHLIGHTED_EDGE_COLOR,
        color: color
    };
};

export const setDraggedFaceColor = color => {
    return {
        type: CHANGE_DRAGGED_FACE_COLOR,
        color: color
    };
};

export const setFixedFaceColor = color => {
    return {
        type: CHANGE_FIXED_FACE_COLOR,
        color: color
    };
};

export const setEditedEdgeColor = color => {
    return {
        type: CHANGE_EDITED_EDGE_COLOR,
        color: color
    };
};

export const setGridSize = size => {
    return {
        type: CHANGE_GRID_SIZE,
        size: size
    };
};

export const setGridTextureSize = size => {
    return {
        type: CHANGE_GRID_TEXTURE_SIZE,
        size: size
    };
};

export const setGridLineWidth = width => {
    return {
        type: CHANGE_GRID_LINE_WIDTH,
        width: width
    };
};

export const showUnitGrid = () => {
    return {
        type: CHANGE_UNIT_GRID_VISIBILITY,
        visibility: Enums.Visibility.VISIBLE
    };
};

export const hideUnitGrid = () => {
    return {
        type: CHANGE_UNIT_GRID_VISIBILITY,
        visibility: Enums.Visibility.HIDDEN
    };
};

export const showPartialViewGridTexture = () => {
    return {
        type: CHANGE_PARTIAL_VIEW_GRID_TEXTURE_VISIBILITY,
        visibility: Enums.Visibility.VISIBLE
    };
};

export const hidePartialViewGridTexture = () => {
    return {
        type: CHANGE_PARTIAL_VIEW_GRID_TEXTURE_VISIBILITY,
        visibility: Enums.Visibility.HIDDEN
    };
};

export const showOptimizationDataMonitor = () => {
    return {
        type: CHANGE_OPTIMIZATION_DATA_MONITOR_VISIBILITY,
        visibility: Enums.Visibility.VISIBLE
    };
};

export const hideOptimizationDataMonitor = () => {
    return {
        type: CHANGE_OPTIMIZATION_DATA_MONITOR_VISIBILITY,
        visibility: Enums.Visibility.HIDDEN
    };
};

export const resumeSolver = () => {
    return {
        type: CHANGE_SOLVER_STATE,
        state: Enums.SolverState.ON
    };
};

export const pauseSolver = () => {
    return {
        type: CHANGE_SOLVER_STATE,
        state: Enums.SolverState.OFF
    };
};

export const loadShapeFile = filename => {
    return {
        type: CHANGE_SHAPE_FILENAME,
        filename: filename
    };
};

export const loadPartialFile = filename => {
    return {
        type: CHANGE_PARTIAL_FILENAME,
        filename: filename
    };
};

export const loadModuleFile = filename => {
    return {
        type: CHANGE_MODULE_FILENAME,
        filename: filename
    };
};

export const setShapeState = state => {
    return {
        type: CHANGE_SHAPE_STATE,
        state: state
    };
};

export const setPartialState = state => {
    return {
        type: CHANGE_PARTIAL_STATE,
        state: state
    };
};

export const setModuleState = state => {
    return {
        type: CHANGE_MODULE_STATE,
        state: state
    };
};

export const setObjectiveFunctionPropertyColor = (objectiveFunctionId, propertyId, color) => {
    return {
        type: CHANGE_OBJECTIVE_FUNCTION_PROPERTY_COLOR,
        objectiveFunctionId: objectiveFunctionId,
        propertyId: propertyId,
        color: color
    };
};

export const setObjectiveFunctionPropertyVisibility = (objectiveFunctionId, propertyId, visibility) => {
    return {
        type: CHANGE_OBJECTIVE_FUNCTION_PROPERTY_VISIBILITY,
        objectiveFunctionId: objectiveFunctionId,
        propertyId: propertyId,
        visibility: visibility
    };
};