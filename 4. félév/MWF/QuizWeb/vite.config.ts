import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'

// https://vite.dev/config/
export default defineConfig({
  server:{
    proxy:{
      '/api' : {
        target: 'https://localhost:7148',
        changeOrigin: true,
        secure: false,
        rewrite: (path) => path.replace(/^\/api/, '')
      },
      '/hubs' : {
        target: 'https://localhost:7148',
        changeOrigin: true,
        secure: false,
        ws: true
      }
    }
  },
  plugins: [vue()]
})
