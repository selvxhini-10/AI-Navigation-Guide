# Integration Guide - Backend & Frontend

This guide explains how to connect the FastAPI backend with the Unreal mobile AR frontend.

## Architecture Overview

```
┌─────────────────────────────────────────┐
│    Unreal Mobile AR App (iOS/Android)   │
│  - ARCharacter (C++)                    │
│  - ARContentManager (C++)               │
│  - Camera & Input System                │
└─────────────┬───────────────────────────┘
              │
              │ HTTP REST API
              │ JSON Serialization
              │
┌─────────────▼───────────────────────────┐
│    FastAPI Backend Server                │
│  - /api/content endpoints                │
│  - /api/markers endpoints                │
│  - Health check endpoints                │
└─────────────┬───────────────────────────┘
              │
              │ SQLite / Database
              │
┌─────────────▼───────────────────────────┐
│    Persistent Storage                    │
│  - AR Content                            │
│  - AR Markers                            │
│  - User Data                             │
└──────────────────────────────────────────┘
```

## Step 1: Configure Backend Connection (Unreal)

### Local Development Setup

1. **In Unreal Editor**:
   - Open `Content Browser`
   - Create a new Blueprint based on `AARCharacter`
   - Name it `BP_ARCharacter`

2. **Set Backend URL**:
   - Select `BP_ARCharacter`
   - In Details panel, find `Backend URL` variable
   - Set to `http://localhost:8000` (for local testing)
   - Or set to your server IP: `http://192.168.1.100:8000`

3. **Save Blueprint**:
   - Set `BP_ARCharacter` as default Pawn in your Game Mode
   - Or manually place it in your level

### Mobile Deployment Setup

1. **For Remote Backend**:
   ```
   Backend URL: http://your-server.com:8000
   ```

2. **Update in Blueprint**:
   - Edit `BP_ARCharacter`
   - Change `Backend URL` to your production server

## Step 2: Start the Backend

### Using Windows PowerShell

```powershell
# Navigate to backend directory
cd .\backend

# Create and activate virtual environment
python -m venv venv
.\venv\Scripts\Activate.ps1

# Install dependencies
pip install -r requirements.txt

# Copy environment file
Copy-Item .env.example .env

# Start the server
python main.py
```

The backend will start on `http://localhost:8000`

### Verify Backend is Running

Open your browser and visit:
```
http://localhost:8000/health
```

You should see:
```json
{
  "status": "healthy",
  "version": "1.0.0",
  "timestamp": "2024-11-15T..."
}
```

## Step 3: Test Frontend-Backend Communication

### In Unreal Editor (Play-in-Editor)

1. **Press Play** to start the game
2. **Check Output Log**:
   - You should see: `"AR Manager initialized with backend: http://localhost:8000"`
   - Followed by: `"Backend Connected!"` (green message)

### Troubleshooting Connection Issues

**Error: "Request failed or invalid response"**
- Verify backend is running: `http://localhost:8000/health`
- Check firewall isn't blocking port 8000
- Verify correct IP/port in Blueprint

**Error: "HTTP Error: 404"**
- Backend API route doesn't exist
- Check endpoint spelling in `ARContentManager.cpp`

**Error: Network timeout**
- Backend may be unresponsive
- Check backend console for errors
- Increase timeout value in HTTP requests

## Step 4: API Communication Flow

### Example: Fetching AR Content

**Unreal (C++)**:
```cpp
void AARCharacter::FetchARContent()
{
    FOnARContentFetched Callback;
    Callback.BindDynamic(this, &AARCharacter::OnFetchContentComplete);
    UARContentManager::FetchARContent(Callback);
}
```

**HTTP Request**:
```
GET /api/content HTTP/1.1
Host: localhost:8000
Content-Type: application/json
```

**Backend (FastAPI)**:
```python
@router.get("/content", response_model=List[ARContent])
async def get_all_content():
    """Get all AR content"""
    return ar_content_store
```

**Response**:
```json
[
  {
    "id": 1,
    "name": "AR Model 1",
    "description": "Sample AR content",
    "content_type": "model",
    "created_at": "2024-11-15T...",
    "updated_at": "2024-11-15T..."
  }
]
```

## Step 5: Data Models & Validation

### Backend (Pydantic Models)

Located in `backend/app/models.py`:

```python
class ARContent(ARContentBase):
    id: int
    created_at: datetime
    updated_at: datetime
```

### Frontend (Unreal Structures)

Located in `frontend/Source/UnrealMobileAR/Public/ARContentManager.h`:

```cpp
USTRUCT(BlueprintType)
struct FARContent
{
    int32 Id;
    FString Name;
    FString Description;
    FString ContentType;
    FString CreatedAt;
};
```

## Step 6: Common API Operations

### Create AR Content

**Blueprint Call**:
```cpp
UARContentManager::CreateARContent(
    TEXT("My AR Model"),
    TEXT("Description of the model"),
    TEXT("model")  // content_type
);
```

**Backend Request**:
```json
POST /api/content
{
  "name": "My AR Model",
  "description": "Description of the model",
  "content_type": "model"
}
```

**Backend Response**:
```json
{
  "id": 1,
  "name": "My AR Model",
  "description": "Description of the model",
  "content_type": "model",
  "created_at": "2024-11-15T10:30:00",
  "updated_at": "2024-11-15T10:30:00"
}
```

### Get Content by ID

**Blueprint Call**:
```cpp
UARContentManager::FetchARContentById(1);
```

**Backend Request**:
```
GET /api/content/1
```

### Delete Content

**Blueprint Call**:
```cpp
UARContentManager::DeleteARContent(1);
```

**Backend Request**:
```
DELETE /api/content/1
```

## Step 7: CORS Configuration

### For Local Development

Default CORS settings allow all origins:
```python
# In backend/main.py
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Allow all
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)
```

### For Production

Restrict to specific origins:
```python
allow_origins=[
    "https://yourdomain.com",
    "https://app.yourdomain.com",
]
```

## Step 8: Error Handling

### Backend Errors

The backend returns appropriate HTTP status codes:

| Code | Meaning | Example |
|------|---------|---------|
| 200 | OK | Successful GET request |
| 201 | Created | Successful POST request |
| 204 | No Content | Successful DELETE request |
| 400 | Bad Request | Invalid JSON payload |
| 404 | Not Found | Content ID doesn't exist |
| 500 | Server Error | Unhandled exception |

### Frontend Error Handling

In Unreal, check the callback:

```cpp
void AARCharacter::OnFetchContentComplete(bool bSuccess, const FString& ErrorMessage)
{
    if (bSuccess)
    {
        // Handle success
    }
    else
    {
        // Handle error: ErrorMessage contains details
    }
}
```

## Step 9: Debugging

### Backend Debugging

```bash
# View logs in real-time
python main.py  # See console output

# Check specific route
curl http://localhost:8000/api/content

# View API documentation
# Visit http://localhost:8000/docs in browser
```

### Frontend Debugging

1. **Output Log**:
   - Window → Developer Tools → Output Log
   - Filter by "AR" or "HTTP"

2. **Breakpoints**:
   - Set breakpoints in `ARContentManager.cpp`
   - Use Visual Studio debugger

3. **Network Inspector** (mobile):
   - Use Unreal's Network Profiler
   - Or use platform-specific debugging

## Step 10: Performance Optimization

### Backend Optimization
- Use connection pooling for database
- Implement caching for frequently accessed content
- Add pagination for large datasets
- Monitor API response times

### Frontend Optimization
- Implement request queuing
- Cache AR content locally
- Use asynchronous loading
- Optimize mesh LODs

## Troubleshooting Checklist

- [ ] Backend is running on correct port (8000)
- [ ] Firewall allows connections on port 8000
- [ ] Unreal client has correct backend URL
- [ ] Network connectivity between client and server
- [ ] Database/storage is accessible
- [ ] No CORS issues (check browser console on web)
- [ ] JSON payloads are valid
- [ ] HTTP methods match (GET, POST, DELETE)
- [ ] Response codes are correct (200, 201, 204, etc)
- [ ] Error messages in backend logs

## Next Steps

1. **Add Authentication**: Implement JWT tokens for security
2. **Database Integration**: Replace in-memory storage with real database
3. **Real-time Updates**: Add WebSocket support for live updates
4. **Scaling**: Deploy backend to cloud service
5. **Mobile Testing**: Test on real iOS/Android devices
6. **Performance Testing**: Load test the backend
7. **Security Audit**: Review security configuration

---

**Happy Developing! 🚀**
