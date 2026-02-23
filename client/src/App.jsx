import { useState, useEffect } from 'react'
import './App.css'

function App() {
  //State Management
  const [view, setView] = useState('home');
  const [stations, setStations] = useState([]);
  const [activeSummary, setActiveSummary] = useState("Waiting for transcription...");
  
  //Selection states for each view
  const [selectedStation, setSelectedStation] = useState(null);
  const [selectedLog, setSelectedLog] = useState(null);

  //Data Fetching
// 1. Define the fetch function so it can be called from anywhere
const fetchStations = async () => {
  try {
    const res = await fetch('http://localhost:8080/stations');
    const data = await res.json();
    setStations(data);
  } catch (err) {
    console.error("Link to C++ failed:", err);
  }
};

// 2. Call it once when the app first loads
useEffect(() => {
  fetchStations();
}, []);

  //Action Handlers
  const handleSave = () => {
    if (!selectedStation) {
      alert("Please select a frequency first!");
      return;
    }
    console.log("Saving to database for:", selectedStation.name);
    //Logic for POST request to Crow goes here
  };

  // 3. Update handleDelete to refresh the list
  const handleDelete = async () => {
    if (selectedLog && window.confirm(`Delete log for ${selectedLog.name}?`)) {
      try {
        const response = await fetch(`http://localhost:8080/api/logs/delete`, {
          method: 'DELETE',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({
            freq: parseFloat(selectedLog.freq), 
            time: selectedLog.time
          }),
        });
  
        if (response.ok) { // This catches 200, 201, 204, etc.
          console.log("Successfully deleted log ID:", selectedLog.id);
          
          // 1. Clear the selection so the details panel goes blank
          setSelectedLog(null);
          
          // 2. REFRESH THE TABLE by asking the server for the updated list
          await fetchStations(); 
          
        } else {
          alert("Failed to delete log on the server.");
        }
      } catch (error) {
        console.error("Connection error:", error);
      }
    }
  };

  const resetView = () => {
    setSelectedStation(null);
    setSelectedLog(null);
    setActiveSummary("Waiting for transcription...");
    setView('home');
  };

  return (
    <div className="container">
      <h1>Radio Scanner & Summarizer</h1>
      
      {/* HOME VIEW */}
      {view === 'home' && (
        <div className="card">
          <button className="main-btn" onClick={() => setView('scanning')}>Scan Now</button>
          <button className="main-btn" onClick={() => setView('database')}>Database</button>
        </div>
      )}

      {/* DATABASE VIEW */}
      {view === 'database' && (
        <div className="database-view-wrapper">
          <div className="scanning-grid">
            <div className="data-box">
              <h3>Saved Logs</h3>
              <ul className="frequency-list">
                {stations.map(s => (
                  <li 
                    key={s.id}
                    onClick={() => setSelectedLog(s)}
                    className={selectedLog?.id === s.id ? "active-station" : ""}
                  >
                    <div className="station-item-content">
                      <span className="freq-tag">{s.freq}</span>
                      <span className="station-name">{s.name}</span>
                    </div>
                  </li>
                ))}
              </ul>
            </div>

            <div className="data-box">
              <h3>Log Details</h3>
              <div className="summary-content">
                {selectedLog ? (
                  <>
                    <p className="summary-text"><strong>Station:</strong> {selectedLog.name}</p>
                    <p className="summary-text"><strong>Frequency:</strong> {selectedLog.freq}</p>
                    <hr style={{ borderColor: '#333', margin: '10px 0' }} />
                    <p className="summary-text">AI Summary: [Stored Transcript Data]</p>
                  </>
                ) : (
                  <p className="summary-text">Select a log to view details</p>
                )}
              </div>
              <div className="action-buttons">
                <button className="sub-btn scan-btn" disabled={!selectedLog}>Fetch</button>
                <button className="sub-btn delete-btn" onClick={handleDelete} disabled={!selectedLog}>Delete</button>
              </div>
            </div>
          </div>
          <div className="button-container">
            <button className="back-btn" onClick={resetView}>Back to Home</button>
          </div>
        </div>
      )}

      {/* SCANNING VIEW */}
      {view === 'scanning' && (
        <div className="scanning-container">
          <div className="scanning-grid">
            <div className="data-box">
              <h3>Live Frequencies</h3>
              <ul className="frequency-list">
                {stations.map(s => (
                  <li 
                    key={s.id}
                    onClick={() => setSelectedStation(s)}
                    className={selectedStation?.id === s.id ? "active-station" : ""}
                  >
                    <div className="station-item-content">
                      <span className="freq-tag">{s.freq}</span>
                      <span className="station-name">{s.name}</span>
                    </div>
                  </li>
                ))}
              </ul>
            </div>

            <div className="data-box">
              <h3>Transmission Summary</h3>
              <div className="summary-content">
                <p className="summary-text">
                  {selectedStation ? `Target: ${selectedStation.freq}` : "Select a frequency"}
                </p>
                <hr style={{ borderColor: '#333', margin: '10px 0' }} />
                <p className="summary-text">{activeSummary}</p>
              </div>
              <div className="action-buttons">
                <button 
                  className="sub-btn scan-btn" 
                  onClick={() => setActiveSummary("Scanning " + (selectedStation?.freq || "") + "...")}
                  disabled={!selectedStation}
                >
                  Scan
                </button>
                <button className="sub-btn save-btn" onClick={handleSave} disabled={!selectedStation}>Save</button>
              </div>
            </div>
          </div>
          <div className="button-container">
            <button className="back-btn" onClick={resetView}>Back to Home</button>
          </div>
        </div>
      )}
    </div>
  )
}

export default App