"""
Test Object Detection Pipeline for Visually Impaired Users
Processes an image, detects objects, draws bounding boxes, generates text descriptions, and creates audio
"""

import cv2
import numpy as np
from pathlib import Path
import json

def load_image(image_path):
    """Load image from file"""
    img = cv2.imread(str(image_path))
    if img is None:
        raise ValueError(f"Could not load image: {image_path}")
    return img

def detect_objects_yolo(image):
    """
    Detect objects using YOLOv5 (lightweight and accurate)
    Returns list of detections: [class, confidence, x, y, width, height]
    """
    try:
        import torch
        
        # Load YOLOv5 model (will download on first run)
        model = torch.hub.load('ultralytics/yolov5', 'yolov5s', pretrained=True)
        model.conf = 0.4  # Confidence threshold
        model.iou = 0.45  # NMS IOU threshold
        
        # Run inference
        results = model(image)
        
        # Extract detections
        detections = []
        for pred in results.xyxy[0]:  # xyxy format: [x1, y1, x2, y2, conf, class]
            x1, y1, x2, y2, conf, cls = pred.tolist()
            
            # Convert to [x, y, width, height]
            x, y = int(x1), int(y1)
            w, h = int(x2 - x1), int(y2 - y1)
            
            class_name = model.names[int(cls)]
            confidence = float(conf)
            
            detections.append({
                'class': class_name,
                'confidence': confidence,
                'bbox': [x, y, w, h]
            })
        
        return detections
    
    except Exception as e:
        print(f"Error with YOLOv5: {e}")
        print("Falling back to placeholder detection...")
        return []

def draw_bounding_boxes(image, detections):
    """Draw bounding boxes and labels on image"""
    output_img = image.copy()
    
    for det in detections:
        x, y, w, h = det['bbox']
        class_name = det['class']
        confidence = det['confidence']
        
        # Draw rectangle
        color = (0, 255, 0)  # Green
        cv2.rectangle(output_img, (x, y), (x + w, y + h), color, 2)
        
        # Draw label background
        label = f"{class_name} {confidence:.2f}"
        font = cv2.FONT_HERSHEY_SIMPLEX
        font_scale = 0.6
        thickness = 2
        
        (text_w, text_h), _ = cv2.getTextSize(label, font, font_scale, thickness)
        cv2.rectangle(output_img, (x, y - text_h - 10), (x + text_w, y), color, -1)
        
        # Draw label text
        cv2.putText(output_img, label, (x, y - 5), font, font_scale, (0, 0, 0), thickness)
    
    return output_img

def generate_description(detections):
    """
    Generate natural language description of detected objects
    Optimized for visually impaired users
    """
    if not detections:
        return "No objects detected in the image."
    
    # Count objects by class
    object_counts = {}
    for det in detections:
        class_name = det['class']
        object_counts[class_name] = object_counts.get(class_name, 0) + 1
    
    # Generate description
    parts = []
    
    total_objects = len(detections)
    parts.append(f"I detected {total_objects} object{'s' if total_objects > 1 else ''}.")
    
    # List objects with positions
    position_descriptions = []
    img_center_x = 320  # Assuming 640px width
    
    for det in detections:
        class_name = det['class']
        x, y, w, h = det['bbox']
        center_x = x + w // 2
        
        # Determine position
        if center_x < img_center_x * 0.4:
            position = "on the left"
        elif center_x > img_center_x * 1.6:
            position = "on the right"
        else:
            position = "in the center"
        
        # Determine relative size
        area_proportion = (w * h) / (640 * 480)
        if area_proportion > 0.3:
            distance = "very close"
        elif area_proportion > 0.1:
            distance = "nearby"
        else:
            distance = "far away"
        
        position_descriptions.append(f"{class_name} {position}, {distance}")
    
    # Create summary
    if len(position_descriptions) <= 3:
        parts.append("Objects are: " + ", ".join(position_descriptions) + ".")
    else:
        # Group by class if many objects
        for obj_class, count in object_counts.items():
            parts.append(f"{count} {obj_class}{'s' if count > 1 else ''}")
    
    return " ".join(parts)

def text_to_speech(text, output_path):
    """
    Convert text to speech audio file
    Uses gTTS (Google Text-to-Speech) - works offline after first download
    """
    try:
        from gtts import gTTS
        import os
        
        # Generate speech
        tts = gTTS(text=text, lang='en', slow=False)
        tts.save(str(output_path))
        
        print(f"✓ Audio saved to: {output_path}")
        return True
        
    except ImportError:
        print("gTTS not installed. Install with: pip install gtts")
        print("Falling back to pyttsx3...")
        
        try:
            import pyttsx3
            
            engine = pyttsx3.init()
            engine.setProperty('rate', 150)  # Speed
            engine.setProperty('volume', 0.9)
            
            # Save to file
            engine.save_to_file(text, str(output_path))
            engine.runAndWait()
            
            print(f"✓ Audio saved to: {output_path}")
            return True
            
        except Exception as e:
            print(f"Error with pyttsx3: {e}")
            print("Please install: pip install pyttsx3")
            return False
    
    except Exception as e:
        print(f"Error generating audio: {e}")
        return False

def main():
    """Main test pipeline"""
    print("=" * 60)
    print("Object Detection Test for Visually Impaired Navigation")
    print("=" * 60)
    
    # Paths
    backend_dir = Path(__file__).parent
    input_image = backend_dir / "test.jpg"
    output_image = backend_dir / "test_detected.jpg"
    output_audio = backend_dir / "detection_audio.mp3"
    output_json = backend_dir / "detection_results.json"
    
    # Check if test image exists
    if not input_image.exists():
        print(f"\n❌ Test image not found: {input_image}")
        print("Please ensure test.jpg exists in the backend directory")
        return
    
    # Step 1: Load image
    print(f"\n📷 Loading image: {input_image}")
    image = load_image(input_image)
    print(f"   Image size: {image.shape[1]}x{image.shape[0]}")
    
    # Step 2: Detect objects
    print("\n🔍 Detecting objects...")
    detections = detect_objects_yolo(image)
    print(f"   Found {len(detections)} objects")
    
    for i, det in enumerate(detections, 1):
        print(f"   {i}. {det['class']} (confidence: {det['confidence']:.2f})")
    
    # Step 3: Draw bounding boxes
    print("\n🖼️  Drawing bounding boxes...")
    output_img = draw_bounding_boxes(image, detections)
    cv2.imwrite(str(output_image), output_img)
    print(f"   Saved to: {output_image}")
    
    # Step 4: Generate text description
    print("\n📝 Generating description...")
    description = generate_description(detections)
    print(f"   Description: {description}")
    
    # Step 5: Convert to audio
    print("\n🔊 Converting to audio...")
    audio_success = text_to_speech(description, output_audio)
    
    # Step 6: Save JSON results
    print("\n💾 Saving results...")
    results = {
        'detections': detections,
        'description': description,
        'audio_file': str(output_audio) if audio_success else None,
        'total_objects': len(detections)
    }
    
    with open(output_json, 'w') as f:
        json.dump(results, f, indent=2)
    print(f"   JSON saved to: {output_json}")
    
    # Summary
    print("\n" + "=" * 60)
    print("✅ Test Complete!")
    print("=" * 60)
    print(f"\nResults:")
    print(f"  - Detected image: {output_image}")
    print(f"  - Audio file: {output_audio}")
    print(f"  - JSON data: {output_json}")
    print(f"\nDescription for audio:")
    print(f'  "{description}"')
    print("\n" + "=" * 60)

if __name__ == "__main__":
    main()
