"""
AI Object Detection Service using YOLOv5
Processes images and generates audio descriptions
"""
import cv2
import numpy as np
from pathlib import Path
from typing import List, Dict, Tuple
import logging
from io import BytesIO
from PIL import Image, ImageDraw, ImageFont

logger = logging.getLogger(__name__)

class AIDetectionService:
    def __init__(self):
        self.model = None
        self.model_loaded = False
        self.output_dir = Path("detected_outputs")
        self.output_dir.mkdir(exist_ok=True)
        
    def load_model(self):
        """Load YOLOv5 model"""
        if self.model_loaded:
            return
        
        try:
            import torch
            logger.info("Loading YOLOv5 model...")
            
            # Load YOLOv5 model
            self.model = torch.hub.load('ultralytics/yolov5', 'yolov5s', pretrained=True)
            self.model.conf = 0.25  # Confidence threshold
            self.model.iou = 0.45   # NMS IOU threshold
            
            self.model_loaded = True
            logger.info("✅ YOLOv5 model loaded successfully")
        except Exception as e:
            logger.error(f"Error loading model: {e}")
            raise
    
    def process_image(self, image_bytes: bytes) -> Dict:
        """
        Process image with AI detection
        
        Args:
            image_bytes: Raw image bytes
            
        Returns:
            Dict containing detections, description, image_url, and audio_url
        """
        try:
            # Load model if not already loaded
            if not self.model_loaded:
                self.load_model()
            
            # Convert bytes to PIL Image
            image = Image.open(BytesIO(image_bytes))
            
            # Convert to RGB if needed
            if image.mode != 'RGB':
                image = image.convert('RGB')
            
            # Run detection
            logger.info("Running object detection...")
            results = self.model(image)
            
            # Extract detections
            detections = []
            for pred in results.xyxy[0]:  # xyxy format: [x1, y1, x2, y2, conf, class]
                x1, y1, x2, y2, conf, cls = pred.tolist()
                
                # Convert to [x, y, width, height]
                x, y = int(x1), int(y1)
                w, h = int(x2 - x1), int(y2 - y1)
                
                class_name = self.model.names[int(cls)]
                confidence = float(conf)
                
                detections.append({
                    'class': class_name,
                    'confidence': confidence,
                    'bbox': [x, y, w, h]
                })
            
            logger.info(f"Found {len(detections)} objects")
            
            # Draw bounding boxes on image
            output_image = self.draw_bounding_boxes(image, detections)
            
            # Save detected image
            image_path = self.output_dir / "detected_image.jpg"
            output_image.save(image_path)
            
            # Generate text description
            description = self.generate_description(detections)
            
            # Generate audio
            audio_path = self.output_dir / "detected_audio.mp3"
            self.text_to_speech(description, audio_path)
            
            return {
                "detections": detections,
                "description": description,
                "image_url": "/api/ai/detected_image.jpg",
                "audio_url": "/api/ai/detected_audio.mp3",
                "count": len(detections)
            }
            
        except Exception as e:
            logger.error(f"Error processing image: {e}")
            raise
    
    def draw_bounding_boxes(self, image: Image.Image, detections: List[Dict]) -> Image.Image:
        """Draw bounding boxes and labels on image"""
        output_img = image.copy()
        draw = ImageDraw.Draw(output_img)
        
        try:
            # Try to use a nice font
            font = ImageFont.truetype("arial.ttf", 20)
        except:
            # Fall back to default font
            font = ImageFont.load_default()
        
        for det in detections:
            x, y, w, h = det['bbox']
            class_name = det['class']
            confidence = det['confidence']
            
            # Draw rectangle
            color = (124, 58, 237)  # Purple
            draw.rectangle([x, y, x + w, y + h], outline=color, width=3)
            
            # Draw label background
            label = f"{class_name} {confidence:.2f}"
            bbox = draw.textbbox((x, y - 25), label, font=font)
            draw.rectangle(bbox, fill=color)
            
            # Draw label text
            draw.text((x, y - 25), label, fill=(255, 255, 255), font=font)
        
        return output_img
    
    def generate_description(self, detections: List[Dict]) -> str:
        """Generate natural language description of detected objects"""
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
        
        # List object types with counts
        object_list = []
        for obj_class, count in object_counts.items():
            if count > 1:
                object_list.append(f"{count} {obj_class}s")
            else:
                object_list.append(f"{count} {obj_class}")
        
        if object_list:
            parts.append("Objects found: " + ", ".join(object_list) + ".")
        
        return " ".join(parts)
    
    def text_to_speech(self, text: str, output_path: Path):
        """Convert text to speech audio file"""
        try:
            from gtts import gTTS
            
            # Generate speech
            tts = gTTS(text=text, lang='en', slow=False)
            tts.save(str(output_path))
            
            logger.info(f"✓ Audio saved to: {output_path}")
            return True
            
        except ImportError:
            logger.warning("gTTS not installed, trying pyttsx3...")
            
            try:
                import pyttsx3
                
                engine = pyttsx3.init()
                engine.setProperty('rate', 150)
                engine.setProperty('volume', 0.9)
                
                engine.save_to_file(text, str(output_path))
                engine.runAndWait()
                
                logger.info(f"✓ Audio saved to: {output_path}")
                return True
                
            except Exception as e:
                logger.error(f"Error generating audio: {e}")
                return False
        
        except Exception as e:
            logger.error(f"Error generating audio: {e}")
            return False

# Global instance
ai_service = AIDetectionService()
