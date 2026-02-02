import { useState, useEffect } from 'react' // Added useEffect here
import './App.css'

function App() {
  //keep a state variable to track which page the user is on
  const [view, setView] = useState('home');
  const [stations, setStations] = useState([]);

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

      
      {view !== 'home' && (
        <div>
          <p>You are now in the {view} view.</p>
          {view === 'database' && (
            <table className="data-table">
              <thead>
                <tr><th>ID</th><th>Station</th><th>Freq</th></tr>
              </thead>
              <tbody>
                {/* 2. Now 'stations' exists, so this .map() will work! */}
                {stations.map(s => (
                  <tr key={s.id}>
                    <td>{s.id}</td>
                    <td>{s.name}</td>
                    <td>{s.freq}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          )}
          <button onClick={() => setView('home')}>Back to Home</button>
        </div>
      )}
    </div>
  )
}


export default App