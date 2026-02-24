import { useState, useEffect } from 'react'
import './App.css'

function App() {
  // State Management
  const [view, setView] = useState('home');
  const [stations, setStations] = useState([]);
  
  // SEPARATED states for live scanning
  const [activeSummary, setActiveSummary] = useState("Waiting for scan...");
  const [activeRawText, setActiveRawText] = useState(""); 
  
  // Selection states for each view
  const [selectedStation, setSelectedStation] = useState(null);
  const [selectedLog, setSelectedLog] = useState(null);
  const [logs, setLogs] = useState([]);

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

  useEffect(() => {
    fetchStations();
    fetchLogs(); 
  }, []);

  // --- THE NEW 2-STEP SCAN HANDLER ---
  const handleScan = async () => {
    if (!selectedStation) return;

    // Reset UI for a new scan
    setActiveRawText("Transcribing audio from " + selectedStation.freq + "...");
    setActiveSummary("Waiting for transcription...");

    try {
      // STEP 1: Ask C++ for the Whisper Transcription
      const transcribeRes = await fetch(`http://localhost:8080/api/transcribe`, {
        method: 'POST',
        body: JSON.stringify({ freq: parseFloat(selectedStation.freq) }),
      });

      if (!transcribeRes.ok) throw new Error("Transcription failed");
      const transcribeData = await transcribeRes.json();
      
      // Update UI independently! (User can now read the raw text)
      const rawText = transcribeData.transcription;
      setActiveRawText(rawText);
      setActiveSummary("Generating AI Summary...");

      // STEP 2: Ask C++ for the Ollama Summary based on that text
      const summaryRes = await fetch(`http://localhost:8080/api/summarize`, {
        method: 'POST',
        body: JSON.stringify({ text: rawText }),
      });

      if (!summaryRes.ok) throw new Error("Summarization failed");
      const summaryData = await summaryRes.json();
      
      // Update UI independently! (Scan complete)
      setActiveSummary(summaryData.summary);

    } catch (error) {
      console.error("Scan error:", error);
      setActiveSummary("Error during scan process.");
      setActiveRawText("Scan failed. Check server console.");
    }
  };

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
          time: Math.floor(Date.now() / 1000), 
          location: "Birmingham, AL", 
          rawT: activeRawText, // <-- Now saves the actual Whisper text!
          summary: activeSummary, // <-- Now saves the actual Ollama summary!
          channelName: selectedStation.name
        }),
      });

      if (response.ok) {
        console.log("Successfully saved log for:", selectedStation.name);
        alert("Log saved successfully!");
        await fetchLogs();
      } else {
        alert("Failed to save log to the server.");
      }
    } catch (error) {
      console.error("Connection error:", error);
    }
  };

  const handleDelete = async () => {
    if (selectedLog && window.confirm(`Delete log for ${selectedLog.name}?`)) {
      try {
        const response = await fetch(`http://localhost:8080/api/logs/delete`, {
          method: 'POST', 
          body: JSON.stringify({
            freq: parseFloat(selectedLog.freq), 
            time: selectedLog.time,
            location: selectedLog.location 
          }),
        });
  
        if (response.ok) { 
          setSelectedLog(null);
          await fetchLogs(); 
        } else {
          alert("Failed to delete log.");
        }
      } catch (error) {
        console.error("Connection error:", error);
      }
    }
  };

  const resetView = () => {
    setSelectedStation(null);
    setSelectedLog(null);
    setActiveSummary("Waiting for scan...");
    setActiveRawText("");
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
                      <span className="freq-tag">{log.freq}</span>
                      <span className="station-name">{log.name || "Unknown"}</span>
                      <span className="station-time" style={{marginLeft: "10px", fontSize: "0.85em", color: "#aaa"}}>
                        {log.time ? new Date(log.time * 1000).toLocaleString(undefined, {
                          month: 'short', day: 'numeric', year: 'numeric', hour: '2-digit', minute: '2-digit'
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
                    <p className="summary-text"><strong>Frequency:</strong> {selectedLog.freq}</p>
                    <p className="summary-text"><strong>Location:</strong> {selectedLog.location}</p>
                    <p className="summary-text"><strong>Time:</strong> {selectedLog.time}</p>
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
                
                {/* DISPLAY SUMMARY FIRST */}
                <p className="summary-text"><strong>AI Summary:</strong> {activeSummary}</p>
                
                {/* DISPLAY RAW TEXT SECOND (Only shows up when it's not empty) */}
                {activeRawText && (
                  <>
                    <br/>
                    <p className="summary-text" style={{ fontSize: "0.85em", color: "#bbb" }}>
                      <em>Raw Text: {activeRawText}</em>
                    </p>
                  </>
                )}

              </div>
              <div className="action-buttons">
                <button className="sub-btn scan-btn" onClick={handleScan} disabled={!selectedStation}>Scan</button>
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