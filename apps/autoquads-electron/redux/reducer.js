import {
    CHANGE_ALGORITHM_TYPE,
    CHANGE_SPLIT_ORIENTATION,
    CHANGE_SHAPE_VIEWPORT_COLOR,
    CHANGE_PARTIAL_VIEWPORT_COLOR,
    CHANGE_SHAPE_COLOR,
    CHANGE_PARTIAL_COLOR,
    CHANGE_SHAPE_WIREFRAME_VISIBILITY,
    CHANGE_PARTIAL_WIREFRAME_VISIBILITY,
    CHANGE_SHAPE_FAT_WIREFRAME_VISIBILITY,
    CHANGE_PARTIAL_FAT_WIREFRAME_VISIBILITY,
    CHANGE_SHAPE_VIEW_VISIBILITY,
    CHANGE_PARTIAL_VIEW_VISIBILITY,
    CHANGE_AUTOCUTS_WEIGHT,    
    CHANGE_DELTA,
    CHANGE_LAMBDA,
    CHANGE_ZETA,
    CHANGE_SEAMLESS_WEIGHT,
    CHANGE_SELECTED_EDGE_SEAMLESS_ANGLE_WEIGHT,
    CHANGE_SELECTED_EDGE_SEAMLESS_LENGTH_WEIGHT,
    CHANGE_SINGULARITY_WEIGHT,
    CHANGE_SINGULARITY_INTERVAL,
    CHANGE_POSITION_WEIGHT,
    CHANGE_GRID_HORIZONTAL_COLOR,
    CHANGE_GRID_VERTICAL_COLOR,
    CHANGE_GRID_BACKGROUND_COLOR1,
    CHANGE_GRID_BACKGROUND_COLOR2,
    CHANGE_HIGHLIGHTED_FACE_COLOR,
    CHANGE_HIGHLIGHTED_EDGE_COLOR,
    CHANGE_DRAGGED_FACE_COLOR,
    CHANGE_FIXED_FACE_COLOR,
    CHANGE_EDITED_EDGE_COLOR,
    CHANGE_GRID_SIZE,
    CHANGE_GRID_TEXTURE_SIZE,
    CHANGE_GRID_LINE_WIDTH,
    CHANGE_UNIT_GRID_VISIBILITY,
    CHANGE_PARTIAL_VIEW_GRID_TEXTURE_VISIBILITY,
    CHANGE_OPTIMIZATION_DATA_MONITOR_VISIBILITY,
    CHANGE_SOLVER_STATE,
    CHANGE_SHAPE_FILENAME,
    CHANGE_PARTIAL_FILENAME,
    CHANGE_MODULE_FILENAME,
    CHANGE_SHAPE_STATE,
    CHANGE_PARTIAL_STATE,    
    CHANGE_MODULE_STATE,
    CHANGE_OBJECTIVE_FUNCTION_PROPERTY_COLOR,
    CHANGE_OBJECTIVE_FUNCTION_PROPERTY_VISIBILITY
} from './actions.js';

import * as EnumsExports from './enums.js';

const INITIAL_STATE = {
    algorithmType: 'autocuts',
    splitOrientation: EnumsExports.SplitOrientation.HORIZONTAL,
    shapeViewportColor: 'rgb(200,200,200)',
    partialViewportColor: 'rgb(200,200,200)',
    shapeColor: 'rgb(220, 220, 220)',
    partialColor: 'rgb(220, 220, 220)',
    shapeWireframeVisibility: EnumsExports.Visibility.VISIBLE,
    partialWireframeVisibility: EnumsExports.Visibility.VISIBLE,
    shapeFatWireframeVisibility: EnumsExports.Visibility.VISIBLE,
    partialFatWireframeVisibility: EnumsExports.Visibility.HIDDEN,
    shapeViewVisibility: EnumsExports.Visibility.VISIBLE,
    partialViewVisibility: EnumsExports.Visibility.VISIBLE,
    autocutsWeight: 1,
    delta: 0.5,
    lambda: 0,
    zeta: 0.95,
    seamlessWeight: 0,
    singularityWeight: 0,
    singularityInterval: 1,
    positionWeight: 100,
    gridHorizontalColor: 'rgb(0,0,0)',
    gridVerticalColor: 'rgb(0,0,0)',
    gridBackgroundColor1: 'rgb(170,170,170)',
    gridBackgroundColor2: 'rgb(170,170,170)',
    highlightedFaceColor: 'rgb(0,255,0)',
    highlightedEdgeColor: 'rgb(0,255,0)',
    draggedFaceColor: 'rgb(0,0,255)',
    fixedFaceColor: 'rgb(255,0,0)',
    editedEdgeColor: 'rgb(0,0,255)',
    gridSize: 7,
    gridTextureSize: 8,
    gridLineWidth: 4,
    unitGridVisibility: EnumsExports.Visibility.HIDDEN,
    partialViewGridTextureVisibility: EnumsExports.Visibility.HIDDEN,
    optimizationDataMonitorVisibility: EnumsExports.Visibility.VISIBLE,
    solverState: EnumsExports.SolverState.OFF,
    shapeFilename: '',
    partialFilename: '',
    moduleFilename: require('path').join(appRoot, 'node-addon.node'),
    shapeState: EnumsExports.LoadState.UNLOADED,
    partialState: EnumsExports.LoadState.UNLOADED,
    moduleState: EnumsExports.LoadState.UNLOADED,
    objectiveFunctionsProperties: [
        {
            objectiveFunctionId: 'Separation',
            propertyId: 'value_per_vertex',
            propertyEffectType: EnumsExports.PropertyEffectType.VERTEX_COLOR,
            objectiveFunctionName: 'Separation',
            propertyName: 'Value Per Vertex',
            color: 'rgb(255,0,0)',
            visibility: EnumsExports.Visibility.HIDDEN,
            weight: 100
        },
        {
            objectiveFunctionId: 'Separation',
            propertyId: 'value_per_edge',
            propertyEffectType: EnumsExports.PropertyEffectType.EDGE_COLOR,
            objectiveFunctionName: 'Separation',
            propertyName: 'Value Per Edge',
            color: 'rgb(255,0,0)',
            visibility: EnumsExports.Visibility.HIDDEN,
            weight: 10000
        },
        {
            objectiveFunctionId: 'Symmetric Dirichlet',
            propertyId: 'value_per_vertex',
            propertyEffectType: EnumsExports.PropertyEffectType.VERTEX_COLOR,
            objectiveFunctionName: 'Symmetric Dirichlet',
            propertyName: 'Value Per Vertex',
            color: 'rgb(255,0,0)',
            visibility: EnumsExports.Visibility.HIDDEN,
            weight: 100
        },
        {
            objectiveFunctionId: 'Seamless',
            propertyId: 'value_per_vertex',
            propertyEffectType: EnumsExports.PropertyEffectType.VERTEX_COLOR,
            objectiveFunctionName: 'Seamless',
            propertyName: 'Value Per Vertex',
            color: 'rgb(255,0,0)',
            visibility: EnumsExports.Visibility.HIDDEN,
            weight: 100
        },
        {
            objectiveFunctionId: 'Seamless',
            propertyId: 'value_per_edge',
            propertyEffectType: EnumsExports.PropertyEffectType.EDGE_COLOR,
            objectiveFunctionName: 'Seamless',
            propertyName: 'Value Per Edge',
            color: 'rgb(255,0,0)',
            visibility: EnumsExports.Visibility.HIDDEN,
            weight: 10000
        },
        {
            objectiveFunctionId: 'Seamless',
            propertyId: 'angle_value_per_edge',
            propertyEffectType: EnumsExports.PropertyEffectType.EDGE_COLOR,
            objectiveFunctionName: 'Seamless',
            propertyName: 'Angle Value Per Edge',
            color: 'rgb(255,0,0)',
            visibility: EnumsExports.Visibility.HIDDEN,
            weight: 10000
        },
        {
            objectiveFunctionId: 'Seamless',
            propertyId: 'length_value_per_edge',
            propertyEffectType: EnumsExports.PropertyEffectType.EDGE_COLOR,
            objectiveFunctionName: 'Seamless',
            propertyName: 'Length Value Per Edge',
            color: 'rgb(255,0,0)',
            visibility: EnumsExports.Visibility.HIDDEN,
            weight: 100000
        },
        {
            objectiveFunctionId: 'Singular Points Position',
            propertyId: 'singularity_weight_per_vertex',
            propertyEffectType: EnumsExports.PropertyEffectType.VERTEX_COLOR,
            objectiveFunctionName: 'Singular Points Position',
            propertyName: 'Singularity Weight Per Vertex',
            color: 'rgb(255,0,0)',
            visibility: EnumsExports.Visibility.HIDDEN,
            weight: 100
        },
        {
            objectiveFunctionId: 'Singular Points Position',
            propertyId: 'positive_angular_defect_singularities_indices',
            propertyEffectType: EnumsExports.PropertyEffectType.SCENE_OBJECTS,
            objectiveFunctionName: 'Singular Points Position',
            propertyName: 'Positive Angular Defect',
            color: 'rgb(0,0,255)',
            visibility: EnumsExports.Visibility.HIDDEN,
            weight: 100
        },
        {
            objectiveFunctionId: 'Singular Points Position',
            propertyId: 'negative_angular_defect_singularities_indices',
            propertyEffectType: EnumsExports.PropertyEffectType.SCENE_OBJECTS,
            objectiveFunctionName: 'Singular Points Position',
            propertyName: 'Negative Angular Defect',
            color: 'rgb(255,0,0)',
            visibility: EnumsExports.Visibility.HIDDEN,
            weight: 100
        }             
    ]
};

function matchObjectiveFunctionProperty(objectivePropertyVisualData, action) {
    if(objectivePropertyVisualData.objectiveFunctionId === action.objectiveFunctionId && 
        objectivePropertyVisualData.propertyId === action.propertyId)
    {
        return true;
    }

    return false;
}

export const reducer = (state = INITIAL_STATE, action) => {
    switch (action.type) {
        case CHANGE_ALGORITHM_TYPE:
            return {
                ...state,
                algorithmType: action.algorithmType
            };
        case CHANGE_SPLIT_ORIENTATION:
            return {
                ...state,
                splitOrientation: action.orientation
            };
        case CHANGE_SHAPE_VIEWPORT_COLOR:
            return {
                ...state,
                shapeViewportColor: action.color
            };
        case CHANGE_PARTIAL_VIEWPORT_COLOR:
            return {
                ...state,
                partialViewportColor: action.color
            };
        case CHANGE_SHAPE_COLOR:
            return {
                ...state,
                shapeColor: action.color
            };
        case CHANGE_PARTIAL_COLOR:
            return {
                ...state,
                partialColor: action.color
            };
        case CHANGE_SHAPE_WIREFRAME_VISIBILITY:
            return {
                ...state,
                shapeWireframeVisibility: action.visibility
            };
        case CHANGE_PARTIAL_WIREFRAME_VISIBILITY:
            return {
                ...state,
                partialWireframeVisibility: action.visibility
            };
        case CHANGE_SHAPE_FAT_WIREFRAME_VISIBILITY:
            return {
                ...state,
                shapeFatWireframeVisibility: action.visibility
            };
        case CHANGE_PARTIAL_FAT_WIREFRAME_VISIBILITY:
            return {
                ...state,
                partialFatWireframeVisibility: action.visibility
            };
        case CHANGE_SHAPE_VIEW_VISIBILITY:
            return {
                ...state,
                shapeViewVisibility: action.visibility
            };
        case CHANGE_PARTIAL_VIEW_VISIBILITY:
            return {
                ...state,
                partialViewVisibility: action.visibility
            };
        case CHANGE_AUTOCUTS_WEIGHT:
            return {
                ...state,
                autocutsWeight: action.weight
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
        case CHANGE_ZETA:
            return {
                ...state,
                zeta: action.zeta
            };            
        case CHANGE_SEAMLESS_WEIGHT:
            return {
                ...state,
                seamlessWeight: action.weight
            };
        case CHANGE_SELECTED_EDGE_SEAMLESS_ANGLE_WEIGHT:
            return {
                ...state,
                selectedEdgeSeamlessAngleWeight: action.weight
            };
        case CHANGE_SELECTED_EDGE_SEAMLESS_LENGTH_WEIGHT:
            return {
                ...state,
                selectedEdgeSeamlessLengthWeight: action.weight
            };            
        case CHANGE_SINGULARITY_WEIGHT:
            return {
                ...state,
                singularityWeight: action.weight
            };
        case CHANGE_SINGULARITY_INTERVAL:
            return {
                ...state,
                singularityInterval: action.interval
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
        case CHANGE_HIGHLIGHTED_EDGE_COLOR:
            return {
                ...state,
                highlightedEdgeColor: action.color
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
        case CHANGE_EDITED_EDGE_COLOR:
            return {
                ...state,
                editedEdgeColor: action.color
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
        case CHANGE_PARTIAL_VIEW_GRID_TEXTURE_VISIBILITY:
            return {
                ...state,
                partialViewGridTextureVisibility: action.visibility
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
        case CHANGE_SHAPE_FILENAME:
            return {
                ...state,
                shapeFilename: action.filename
            };
        case CHANGE_PARTIAL_FILENAME:
            return {
                ...state,
                partialFilename: action.filename
            };            
        case CHANGE_MODULE_FILENAME:
            return {
                ...state,
                moduleFilename: action.filename
            };
        case CHANGE_SHAPE_STATE:
            return {
                ...state,
                shapeState: action.state
            };
        case CHANGE_PARTIAL_STATE:
            return {
                ...state,
                partialState: action.state
            };
        case CHANGE_MODULE_STATE:
            return {
                ...state,
                moduleState: action.state
            };
        case CHANGE_OBJECTIVE_FUNCTION_PROPERTY_COLOR:
            var newState = {
                ...state
            }   
            return newState;
        case CHANGE_OBJECTIVE_FUNCTION_PROPERTY_VISIBILITY:
            var newState = {
                ...state
            }
            return newState;
        default:
            return state;
    }
};