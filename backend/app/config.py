"""
Application configuration settings
"""
from pydantic_settings import BaseSettings
from typing import Optional

class Settings(BaseSettings):
    """Application settings"""
    database_url: str = "sqlite:///./ar_backend.db"
    api_port: int = 8000
    api_host: str = "0.0.0.0"
    debug: bool = True
    cors_origins: list[str] = ["*"]
    
    class Config:
        env_file = ".env"
        env_file_encoding = "utf-8"

settings = Settings()
