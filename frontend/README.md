# Unreal Mobile AR Frontend

Unreal Engine 5.4 mobile AR application frontend for cross-platform AR experiences.

## Project Structure

```
frontend/
├── UnrealMobileAR.uproject          # Project configuration
├── Source/
│   ├── UnrealMobileAR/              # Main game module
│   │   ├── Public/
│   │   │   ├── UnrealMobileAR.h
│   │   │   ├── ARCharacter.h        # Player character class
│   │   │   └── ARContentManager.h   # Backend API manager
│   │   ├── Private/
│   │   │   ├── UnrealMobileAR.cpp
│   │   │   ├── ARCharacter.cpp
│   │   │   ├── ARContentManager.cpp
│   │   │   └── UnrealMobileARGameMode.cpp
│   │   ├── UnrealMobileAR.Build.cs
│   │   └── UnrealMobileARGameMode.h
│   ├── UnrealMobileARTarget.Target.cs
│   └── UnrealMobileAREditor.Target.cs
├── Binaries/                         # Compiled binaries
├── Intermediate/                     # Temporary build files
├── Saved/                            # Saved data (maps, configs, logs)
├── Content/                          # Game assets (auto-generated)
└── README.md                         # This file
```

## Requirements

- **Unreal Engine**: 5.4 or later
- **Platform Support**: 
  - iOS 14.0+
  - Android 8.0+
- **Dependencies**:
  - Enhanced Input System plugin
  - Apple ARKit plugin (iOS)
  - Google ARCore plugin (Android)

## Setup Instructions

### 1. Prerequisites

Ensure you have:
- Unreal Engine 5.4 installed
- Visual Studio 2022 (Windows) or Xcode (Mac)
- iOS or Android SDK installed

### 2. Open the Project

```bash
# On Windows
cd frontend
UnrealMobileAR.uproject
```

Or open Unreal Editor and select the project file.

### 3. Configure Backend Connection

In the editor:
1. Open `BP_ARCharacter` (or your character blueprint)
2. Set `Backend URL` to your FastAPI backend address
   - Local development: `http://localhost:8000`
   - Remote: `http://your-server.com:8000`

### 4. Build for Mobile

#### iOS Build
1. File → Project Settings → Platforms → iOS
2. Configure signing certificates
3. File → Package Project → iOS → Choose destination folder

#### Android Build
1. File → Project Settings → Platforms → Android
2. Configure Android SDK/NDK paths
3. File → Package Project → Android (ARM64)

## Key Classes

### ARContentManager (C++)
Manages HTTP communication with the FastAPI backend.

**Key Functions:**
- `InitializeARManager(BackendUrl)` - Initialize with backend URL
- `FetchARContent()` - Get all AR content from backend
- `CreateARContent(Name, Description, Type)` - Create new AR content
- `DeleteARContent(ContentId)` - Delete AR content

### ARCharacter (C++)
Player character that handles AR interactions and backend communication.

**Features:**
- Camera component for AR view
- Input handling (movement, interaction)
- AR content management
- Backend connectivity

## API Integration

The frontend communicates with the FastAPI backend through REST API endpoints:

```
Backend Base URL: http://localhost:8000

Endpoints:
- GET  /health              - Health check
- GET  /api/content         - List all AR content
- POST /api/content         - Create AR content
- GET  /api/content/{id}    - Get specific content
- PUT  /api/content/{id}    - Update AR content
- DELETE /api/content/{id}  - Delete AR content
- GET  /api/markers         - List AR markers
- POST /api/markers         - Create AR marker
```

## Development

### Building from Source

```bash
cd frontend
GenerateProjectFiles.bat
UnrealMobileAR.sln  # Open in Visual Studio
# Build solution in Release mode
```

### Hot Reload

During development, use Unreal's Live Coding:
1. Edit C++ code
2. Ctrl + Alt + F11 to recompile
3. Changes apply without restarting editor

### Debugging

1. Set breakpoints in Visual Studio
2. Attach debugger: Debug → Attach to Process
3. Run the game in editor (PIE)

## Mobile Deployment

### iOS
1. Ensure provisioning profiles are configured
2. Create archive: Product → Archive
3. Sign and upload to App Store

### Android
1. Ensure keystore is configured
2. Package as APK or AAB
3. Sign and upload to Google Play Store

## Common Issues

### Backend Connection Failed
- Verify backend URL in character blueprint
- Check firewall/network settings
- Ensure backend is running: `python main.py`

### AR Features Not Working
- Check AR plugin is enabled in project settings
- Verify device supports ARKit/ARCore
- Check camera permissions in mobile settings

### Build Errors
- Clean Intermediate folder
- Delete Binaries folder
- Regenerate project files
- Rebuild solution

## Performance Tips

1. Use mobile-friendly assets (LODs)
2. Optimize AR marker detection
3. Use asynchronous API calls
4. Profile with mobile profiler

## License

MIT

## Support

For issues and questions, refer to the backend README and official Unreal/FastAPI documentation.
