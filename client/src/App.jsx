import { useState, useEffect } from 'react'
import './App.css'

function App() {
  // State Management
  const [view, setView] = useState('home');
  const [stations, setStations] = useState([]);
  const [activeSummary, setActiveSummary] = useState("Waiting for transcription...");
  
  // Selection states for each view
  const [selectedStation, setSelectedStation] = useState(null);
  const [selectedLog, setSelectedLog] = useState(null);
  const [logs, setLogs] = useState([]);

  // 1. Define BOTH fetch functions first
  const fetchLogs = async () => {
    try {
      const res = await fetch('http://localhost:8080/api/logs'); 
      const data = await res.json();
      setLogs(data);
    } catch (err) {
      console.error("Failed to fetch logs:", err);
    }
  };

  const fetchStations = async () => {
    try {
      const res = await fetch('http://localhost:8080/stations');
      const data = await res.json();
      setStations(data);
    } catch (err) {
      console.error("Link to C++ failed:", err);
    }
  };

  // 2. Call them in a SINGLE useEffect right after they are defined
  useEffect(() => {
    fetchStations();
    fetchLogs(); 
  }, []);

  // Action Handlers
  const handleSave = async () => {
    if (!selectedStation) {
      alert("Please select a frequency first!");
      return;
    }

    try {
      const response = await fetch(`http://localhost:8080/api/logs/save`, {
        method: 'POST',
        body: JSON.stringify({
          freq: parseFloat(selectedStation.freq),
          time: Math.floor(Date.now() / 1000), // Get current Unix timestamp
          location: "Birmingham, AL", // Hardcoded for now, can be dynamic later!
          rawT: "Raw audio transmission captured...", // Placeholder for Whisper transcription
          summary: activeSummary,
          channelName: selectedStation.name
        }),
      });

      if (response.ok) {
        console.log("Successfully saved log for:", selectedStation.name);
        alert("Log saved successfully!");
        
        // Refresh the database table in the background so it's ready when you switch views
        await fetchLogs();
      } else {
        alert("Failed to save log to the server.");
      }
    } catch (error) {
      console.error("Connection error:", error);
    }
  };

  // 3. Update handleDelete to include location for the composite key
  const handleDelete = async () => {
    if (selectedLog && window.confirm(`Delete log for ${selectedLog.name}?`)) {
      try {
        const response = await fetch(`http://localhost:8080/api/logs/delete`, {
          method: 'POST', 
          body: JSON.stringify({
            freq: parseFloat(selectedLog.freq), 
            time: selectedLog.time,
            location: selectedLog.location // <-- Added location to satisfy the C++ composite key!
          }),
        });
  
        if (response.ok) { 
          console.log("Successfully deleted log ID:", selectedLog.id);
          setSelectedLog(null);
          await fetchLogs(); 
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
      <h1>AetherGuard</h1>
      
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
                {logs.map(log => (
                  <li 
                    key={log.id}
                    onClick={() => setSelectedLog(log)}
                    className={selectedLog?.id === log.id ? "active-station" : ""}
                  >
                    <div className="station-item-content">
                      <span className="freq-tag">{Number(log.freq).toFixed(3)} MHz</span>
                      <span className="station-name">{log.name || "Unknown"}</span>
                      <span className="station-time" style={{marginLeft: "10px", fontSize: "0.85em", color: "#aaa"}}>
                        {log.time ? new Date(log.time * 1000).toLocaleString(undefined, {
                          month: 'short',
                          day: 'numeric',
                          year: 'numeric',
                          hour: '2-digit',
                          minute: '2-digit'
                          }) : "Unknown"}
                      </span>
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
                    <p className="summary-text"><strong>Frequency:</strong> {Number(selectedLog.freq).toFixed(3)} MHz</p>
                    <p className="summary-text"><strong>Location:</strong> {selectedLog.location}</p>
                    
                    {/* NEW: Normal Time + Unix Time displayed together */}
                    <p className="summary-text">
                      <strong>Time:</strong> {selectedLog.time ? new Date(selectedLog.time * 1000).toLocaleString(undefined, {
                          month: 'short',
                          day: 'numeric',
                          year: 'numeric',
                          hour: '2-digit',
                          minute: '2-digit'
                      }) : "Unknown"} <span style={{ fontSize: "0.85em", color: "#aaa" }}>(Unix: {selectedLog.time})</span>
                    </p>
                    
                    <hr style={{ borderColor: '#333', margin: '10px 0' }} />
                    <p className="summary-text"><strong>AI Summary:</strong> {selectedLog.summary || "No summary available"}</p>
                    <br/>
                    <p className="summary-text" style={{ fontSize: "0.85em", color: "#bbb" }}>
                      <em>Raw Text: {selectedLog.rawT || "No raw text available"}</em>
                    </p>
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
                      <span className="freq-tag">{Number(s.freq).toFixed(3)} MHz</span>
                      <span className="station-name">{s.name}</span>
                    </div>
                  </li>
                ))}
              </ul>
            </div>

            <div className="data-box">
              <h3>Transmission Summary</h3>
              <div className="summary-content">
                {/* Fixed the target display below */}
                <p className="summary-text">
                  {selectedStation ? `Target: ${Number(selectedStation.freq).toFixed(3)} MHz` : "Select a frequency"}
                </p>
                <hr style={{ borderColor: '#333', margin: '10px 0' }} />
                <p className="summary-text">{activeSummary}</p>
              </div>
              <div className="action-buttons">
                {/* Ensure the "Scanning..." message formats the frequency cleanly too */}
                <button 
                  className="sub-btn scan-btn" 
                  onClick={() => setActiveSummary("Scanning " + (selectedStation ? Number(selectedStation.freq).toFixed(3) + " MHz" : "") + "...")}
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