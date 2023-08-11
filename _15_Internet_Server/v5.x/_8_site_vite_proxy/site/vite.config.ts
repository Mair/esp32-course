import { defineConfig } from "vite";
import react from "@vitejs/plugin-react-swc";

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react()],
  server: {
    proxy: {
      "/api": {
        target: "http://my-esp32.local",
        changeOrigin: true,
      },
      "/ws": {
        target: "http://my-esp32.local",
        ws: true,
      },
    },
  },
});
