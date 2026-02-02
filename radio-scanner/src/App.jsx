import { useState } from 'react'
import './App.css'

function App() {
  //keep a state variable to track which page the user is on
  const [view, setView] = useState('home');

  return (
    <div className="container">
      <h1>Radio Scanner & Summarizer</h1>
      
      {view === 'home' && (
        <div className="card">
          <button className="main-btn" onClick={() => setView('scanning')}>
            Scan Now
          </button>
          
          <button className="main-btn" onClick={() => setView('database')}>
            Database
          </button>
        </div>
      )}

      {/*Logic to show back button if we leave home */}
      {view !== 'home' && (
        <div>
          <p>You are now in the {view} view.</p>
          <button onClick={() => setView('home')}>Back to Home</button>
        </div>
      )}
    </div>
  )
}

export default App