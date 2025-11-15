"""
Navigation and GPS guidance endpoints
Integrates with Google Maps API for route planning and turn-by-turn guidance
"""
from fastapi import APIRouter, status, HTTPException, Query
from app.models import (
    NavigationGuidance, GPSLocation, ObstacleAlert
)
from typing import List, Optional
from datetime import datetime

router = APIRouter()

# Mock storage for navigation sessions and routes
active_routes: List[dict] = []
obstacle_alerts_store: List[dict] = []
route_id_counter = 1
alert_id_counter = 1

@router.post("/navigation/start-route", status_code=status.HTTP_201_CREATED)
async def start_navigation_route(
    origin: GPSLocation,
    destination: GPSLocation,
    session_id: Optional[str] = None
):
    """
    Start a new navigation route from origin to destination
    Integrates with Google Maps API for optimal routing
    
    Args:
        origin: Starting GPS location
        destination: Target destination GPS location
        session_id: Optional session identifier
    
    Returns:
        Navigation guidance with step-by-step instructions
    """
    global route_id_counter
    
    route_id = f"route_{route_id_counter}"
    route_id_counter += 1
    
    # Mock Google Maps integration
    # In production, this would call Google Maps API
    mock_instructions = [
        "Head north on Main Street for 500 meters",
        "Turn right onto Market Avenue",
        "Continue for 300 meters to destination",
        "Destination is on your left"
    ]
    
    mock_waypoints = [
        origin,
        GPSLocation(latitude=origin.latitude + 0.005, longitude=origin.longitude),
        GPSLocation(latitude=origin.latitude + 0.010, longitude=origin.longitude + 0.005),
        destination
    ]
    
    # Calculate mock distance and duration
    mock_distance = 1200  # meters
    mock_duration = 900  # seconds (15 minutes)
    
    route = {
        "route_id": route_id,
        "session_id": session_id,
        "origin": origin.dict(),
        "destination": destination.dict(),
        "instructions": mock_instructions,
        "distance_remaining": mock_distance,
        "duration_remaining": mock_duration,
        "current_step": 0,
        "waypoints": [wp.dict() for wp in mock_waypoints],
        "status": "active",
        "created_at": datetime.utcnow()
    }
    
    active_routes.append(route)
    
    return {
        "route_id": route_id,
        "status": "started",
        "total_distance": mock_distance,
        "estimated_duration": mock_duration,
        "instructions": mock_instructions
    }

@router.get("/navigation/route/{route_id}", response_model=dict)
async def get_route_status(route_id: str):
    """
    Get current status and guidance for an active route
    
    Args:
        route_id: Route identifier
    
    Returns:
        Current navigation guidance
    """
    for route in active_routes:
        if route["route_id"] == route_id:
            return route
    
    raise HTTPException(status_code=404, detail=f"Route {route_id} not found")

@router.put("/navigation/route/{route_id}/update-location")
async def update_user_location(route_id: str, current_location: GPSLocation):
    """
    Update user's current location and get next instruction
    
    Args:
        route_id: Route identifier
        current_location: Current GPS location
    
    Returns:
        Updated navigation guidance with next instruction
    """
    for route in active_routes:
        if route["route_id"] == route_id:
            route["current_location"] = current_location.dict()
            
            # Mock distance calculation
            # In production, calculate actual distance to destination
            route["distance_remaining"] = max(0, route["distance_remaining"] - 50)
            route["duration_remaining"] = max(0, route["duration_remaining"] - 30)
            
            # Auto-advance to next instruction if close enough
            if route["distance_remaining"] < 100 and route["current_step"] < len(route["instructions"]) - 1:
                route["current_step"] += 1
            
            if route["distance_remaining"] == 0:
                route["status"] = "completed"
            
            return {
                "route_id": route_id,
                "current_instruction": route["instructions"][route["current_step"]],
                "step_number": route["current_step"] + 1,
                "total_steps": len(route["instructions"]),
                "distance_remaining": route["distance_remaining"],
                "duration_remaining": route["duration_remaining"],
                "status": route["status"]
            }
    
    raise HTTPException(status_code=404, detail=f"Route {route_id} not found")

@router.post("/navigation/route/{route_id}/end", status_code=status.HTTP_200_OK)
async def end_navigation_route(route_id: str):
    """
    End an active navigation route
    
    Args:
        route_id: Route identifier
    
    Returns:
        Route completion summary
    """
    for route in active_routes:
        if route["route_id"] == route_id:
            route["status"] = "completed"
            route["completed_at"] = datetime.utcnow()
            
            return {
                "route_id": route_id,
                "status": "completed",
                "message": "Route navigation completed",
                "completed_at": route["completed_at"]
            }
    
    raise HTTPException(status_code=404, detail=f"Route {route_id} not found")

@router.get("/navigation/active-routes")
async def get_active_routes():
    """Get all active navigation routes"""
    return [route for route in active_routes if route["status"] == "active"]

@router.post("/navigation/obstacle-alert", status_code=status.HTTP_201_CREATED)
async def report_obstacle(
    alert_type: str,
    severity: str,
    description: str,
    location: Optional[GPSLocation] = None,
    detected_objects: List[dict] = []
):
    """
    Report an obstacle or hazard detected on the route
    
    Args:
        alert_type: Type of obstacle (obstacle, hazard, pedestrian, vehicle, etc.)
        severity: Alert severity (low, medium, high, critical)
        description: Detailed description
        location: GPS location of obstacle
        detected_objects: Related detected objects from camera
    
    Returns:
        Obstacle alert record
    """
    global alert_id_counter
    
    alert_id = f"alert_{alert_id_counter}"
    alert_id_counter += 1
    
    alert = {
        "alert_id": alert_id,
        "timestamp": datetime.utcnow(),
        "alert_type": alert_type,
        "severity": severity,
        "description": description,
        "location": location.dict() if location else None,
        "detected_objects": detected_objects,
        "status": "active"
    }
    
    obstacle_alerts_store.append(alert)
    
    return alert

@router.get("/navigation/obstacles")
async def get_active_obstacles():
    """Get all active obstacle alerts"""
    return [alert for alert in obstacle_alerts_store if alert["status"] == "active"]

@router.put("/navigation/obstacle/{alert_id}/resolve")
async def resolve_obstacle_alert(alert_id: str):
    """Resolve an obstacle alert after it's cleared"""
    for alert in obstacle_alerts_store:
        if alert["alert_id"] == alert_id:
            alert["status"] = "resolved"
            alert["resolved_at"] = datetime.utcnow()
            return alert
    
    raise HTTPException(status_code=404, detail=f"Alert {alert_id} not found")
