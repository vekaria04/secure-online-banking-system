import React, { useState } from "react";
import axios from "axios";
import { Container, Typography, TextField, Button, Alert, Box } from "@mui/material";
import { useNavigate } from "react-router-dom";

function Deposit() {
  const [amount, setAmount] = useState("");
  const [message, setMessage] = useState("");
  const navigate = useNavigate();
  const accountId = localStorage.getItem("accountId");

  async function handleSubmit(e) {
    e.preventDefault();
    setMessage("");
    try {
      await axios.post("http://localhost:3001/transaction", {
        type: "Deposit",
        accountId: parseInt(accountId),
        amount: parseFloat(amount)
      });
      setMessage("Deposit successful");
    } catch (err) {
      setMessage("Deposit failed");
    }
  }

  return (
    <Container maxWidth="sm" sx={{ mt: 4 }}>
      <Typography variant="h4" gutterBottom>Deposit Funds</Typography>
      {message && <Alert severity="info" sx={{ mb: 2 }}>{message}</Alert>}
      <Box component="form" onSubmit={handleSubmit} noValidate sx={{ display: "flex", flexDirection: "column", gap: 2 }}>
        <TextField 
          label="Amount" 
          type="number" 
          value={amount} 
          onChange={(e) => setAmount(e.target.value)} 
          required 
        />
        <Button variant="contained" type="submit">Submit Deposit</Button>
        <Button variant="outlined" onClick={() => navigate("/dashboard")}>Back to Dashboard</Button>
      </Box>
    </Container>
  );
}

export default Deposit;
