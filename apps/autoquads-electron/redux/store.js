import { createStore } from '../web_modules/redux.js';
import { reducer } from './reducer.js';

export const store = createStore(
  reducer
);