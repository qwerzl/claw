import asyncio
import websockets
import msgpack

async def handle_connection(websocket, path):
    print("Client connected")
    try:
        while True:
            message = await websocket.recv()
            if isinstance(message, bytes):
                print(msgpack.unpackb(message))
                break
    except websockets.ConnectionClosed:
        print("Client disconnected")

async def main():
    async with websockets.serve(handle_connection, "0.0.0.0", 8080):
        print("Server started on ws://0.0.0.0:8080")
        await asyncio.Future()  # Run forever

if __name__ == "__main__":
    asyncio.run(main())