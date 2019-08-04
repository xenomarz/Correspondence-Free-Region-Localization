import { LitElement, html } from 'lit-element';
import '@vaadin/vaadin-button';

class AutoquadsView extends LitElement {
  render(){
    return html`
      <h1>Hello</h1>
      <vaadin-button>Hello</vaadin-button>      
    `;
  }
}

customElements.define('autoquads-view', AutoquadsView);